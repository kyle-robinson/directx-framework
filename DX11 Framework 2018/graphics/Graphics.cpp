#include "Graphics.h"
#include "Blender.h"
#include "Sampler.h"
#include "Stencil.h"
#include "Viewport.h"
#include "ModelData.h"
#include "SwapChain.h"
#include "Rasterizer.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"
#include "../resource.h"
#include <fstream>

bool Graphics::Initialize( HWND hWnd, int width, int height )
{
	windowWidth = width;
	windowHeight = height;

	if ( !InitializeDirectX( hWnd ) )
		return false;

	if ( !InitializeShaders() )
		return false;

	if ( !InitializeScene() )
		return false;

    imgui.Initialize( hWnd, device.Get(), context.Get() );

	return true;
}

void Graphics::BeginFrame()
{
	// clear render target
    renderTarget->BindAsTexture( *this, depthStencil.get(), sceneParams.clearColor );
    depthStencil->ClearDepthStencil( *this );

	// set render state
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    stencilStates["Off"]->Bind( *this );
    blendState->Bind( *this );
    if ( viewportParams.useFull )
        viewports["Full"]->Bind( *this );
    if ( viewportParams.useLeft )
    {
        viewportParams.useLeft = false;
        cameraToUse = "Main";
        viewports["Left"]->Bind( *this );
    }
    if ( viewportParams.useRight )
    {
        viewportParams.useRight = false;
        cameraToUse = "Sub";
        viewports["Right"]->Bind( *this );
    }
    sceneParams.rasterizerSolid ? rasterizerStates["Solid"]->Bind( *this ) : rasterizerStates["Wireframe"]->Bind( *this );
    sceneParams.samplerAnisotropic ? samplerStates["Anisotropic"]->Bind( *this ) : samplerStates["Point"]->Bind( *this );

    // setup constant buffers
    if ( !cb_vs_fog.ApplyChanges() ) return;
	context->VSSetConstantBuffers( 1, 1, cb_vs_fog.GetAddressOf() );
	context->PSSetConstantBuffers( 1, 1, cb_vs_fog.GetAddressOf() );

    cb_ps_light.data.useQuad = false;
    light.UpdateConstantBuffer( cb_ps_light );
	if ( !cb_ps_light.ApplyChanges() ) return;
	context->PSSetConstantBuffers( 2, 1, cb_ps_light.GetAddressOf() );

    cb_ps_scene.data.alphaFactor = sceneParams.alphaFactor;
    cb_ps_scene.data.useTexture = sceneParams.useTexture;
    if ( !cb_ps_scene.ApplyChanges() ) return;
	context->PSSetConstantBuffers( 3, 1, cb_ps_scene.GetAddressOf() );
}

void Graphics::RenderFrame()
{   
    // setup sprite masking
    if ( sceneParams.useMask ) 
    {
        stencilStates["Mask"]->Bind( *this );
        context->VSSetShader( vertexShader_2D.GetShader(), NULL, 0 );
        context->IASetInputLayout( vertexShader_2D.GetInputLayout() );
        context->PSSetShader( pixelShader_2D_discard.GetShader(), NULL, 0 );
        sceneParams.circleMask ? circle.Draw( camera2D.GetWorldOrthoMatrix() ) : square.Draw( camera2D.GetWorldOrthoMatrix() );
        stencilStates["Write"]->Bind( *this );
    }
	
    // setup shaders
	context->VSSetShader( vertexShader_light.GetShader(), NULL, 0 );
	context->IASetInputLayout( vertexShader_light.GetInputLayout() );
    context->PSSetShader( pixelShader_light.GetShader(), NULL, 0 );


    // render models
    for ( unsigned int i = 0; i < renderables.size(); i++ )
        renderables.at( i ).Draw( cameras[cameraToUse]->GetViewMatrix(), cameras[cameraToUse]->GetProjectionMatrix() );

    // draw primitves
    cube.Draw( cb_vs_matrix, boxTexture.Get() );
    ground.DrawInstanced( cb_vs_matrix, cb_ps_light, grassTexture.Get() );

	context->PSSetShader( pixelShader_noLight.GetShader(), NULL, 0 );
	light.Draw( cameras[cameraToUse]->GetViewMatrix(), cameras[cameraToUse]->GetProjectionMatrix() );
}

void Graphics::EndFrame()
{
    // set and clear back buffer
    backBuffer->BindAsBuffer( *this, sceneParams.clearColor );

    // render to fullscreen texture
    context->PSSetShaderResources( 0, 1, renderTarget->GetShaderResourceViewPtr() );
    fullscreen.SetupBuffers( vertexShader_full, pixelShader_full, cb_vs_fullscreen, sceneParams.multiView );
    Bind::Rasterizer::DrawSolid( *this, fullscreen.ib_full.IndexCount() ); // always draw as solid

    // display imgui
    imgui.BeginRender();
    imgui.RenderMainWindow( *this, context.Get() );
    imgui.RenderLightWindow( light, cb_ps_light );
    imgui.RenderFogWindow( cb_vs_fog );
    imgui.RenderModelWindow( renderables );
    imgui.RenderCameraWindow( *cameras[cameraToUse], windowWidth, windowHeight );
    imgui.EndRender();

    // unbind rtv and srv
    renderTarget->BindAsNull( *this );
    backBuffer->BindAsNull( *this );
    
    // display frame
	HRESULT hr = swapChain->GetSwapChain()->Present( 1, NULL );
	if ( FAILED( hr ) )
	{
		hr == DXGI_ERROR_DEVICE_REMOVED ?
            ErrorLogger::Log( device->GetDeviceRemovedReason(), "Swap Chain. Graphics device removed!" ) :
            ErrorLogger::Log( hr, "Swap Chain failed to render frame!" );
		exit( -1 );
	}
}

void Graphics::Update( float dt )
{
    // model transformations
    //nanosuit.AdjustRotation( XMFLOAT3( 0.0f, 0.001f * dt, 0.0f ) );

    static float timer = 0.0f;
    static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount64();
	if ( dwTimeStart == 0 ) dwTimeStart = dwTimeCur;
	timer = ( dwTimeCur - dwTimeStart ) / 1000.0f;

    // primitive transformations
    cube.Update( timer );
    ground.UpdateInstanced( 5, 6, 8, 60 );

    // camera viewing
    if ( cameraToUse == "Sub" )
    {
        XMFLOAT3 positions = renderables.at( 0 ).GetPositionFloat3();
        positions.x = positions.x + renderables.at( 0 ).GetScaleFloat3().x / 2.0f;
        positions.y = positions.y + renderables.at( 0 ).GetScaleFloat3().y / 2.0f;
        positions.z = positions.z + renderables.at( 0 ).GetScaleFloat3().z / 2.0f;
        static int radius = 20.0f;
        cameras[cameraToUse]->SetLookAtPos( positions );
        if ( ( cameras[cameraToUse]->GetPositionFloat3().x - positions.x ) *
            ( cameras[cameraToUse]->GetPositionFloat3().x - positions.x ) +
            ( cameras[cameraToUse]->GetPositionFloat3().y - positions.y ) *
            ( cameras[cameraToUse]->GetPositionFloat3().y - positions.y ) +
            ( cameras[cameraToUse]->GetPositionFloat3().z - positions.z ) *
            ( cameras[cameraToUse]->GetPositionFloat3().z - positions.z ) <= radius * radius )
            sceneParams.cameraCollision = true;
        else
            sceneParams.cameraCollision = false;
    }
}

UINT Graphics::GetWidth() const noexcept
{
    return windowWidth;
}

UINT Graphics::GetHeight() const noexcept
{
    return windowHeight;
}

bool Graphics::InitializeDirectX( HWND hWnd )
{
    try
    {
        swapChain = std::make_shared<Bind::SwapChain>( *this, context.GetAddressOf(), device.GetAddressOf(), hWnd );
        backBuffer = std::make_shared<Bind::RenderTarget>( *this, swapChain->GetSwapChain() );
        renderTarget = std::make_shared<Bind::RenderTarget>( *this );

        depthStencil = std::make_shared<Bind::DepthStencil>( *this );
        blendState = std::make_shared<Bind::Blender>( *this );

        viewports.emplace( "Full", std::make_shared<Bind::Viewport>( *this, Bind::Viewport::Side::Full ) );
        viewports.emplace( "Left", std::make_shared<Bind::Viewport>( *this, Bind::Viewport::Side::Left ) );
        viewports.emplace( "Right", std::make_shared<Bind::Viewport>( *this, Bind::Viewport::Side::Right ) );

        stencilStates.emplace( "Off", std::make_shared<Bind::Stencil>( *this, Bind::Stencil::Mode::Off ) );
        stencilStates.emplace( "Mask", std::make_shared<Bind::Stencil>( *this, Bind::Stencil::Mode::Mask ) );
        stencilStates.emplace( "Write", std::make_shared<Bind::Stencil>( *this, Bind::Stencil::Mode::Write ) );

        rasterizerStates.emplace( "Solid", std::make_shared<Bind::Rasterizer>( *this, true, false ) );
        rasterizerStates.emplace( "Wireframe", std::make_shared<Bind::Rasterizer>( *this, false, true ) );

        samplerStates.emplace( "Anisotropic", std::make_shared<Bind::Sampler>( *this, Bind::Sampler::Type::Anisotropic ) );
        samplerStates.emplace( "Bilinear", std::make_shared<Bind::Sampler>( *this, Bind::Sampler::Type::Bilinear ) );
        samplerStates.emplace( "Point", std::make_shared<Bind::Sampler>( *this, Bind::Sampler::Type::Point ) );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }

    return true;
}

bool Graphics::InitializeShaders()
{
    try
    {
        /*   MODELS   */
        D3D11_INPUT_ELEMENT_DESC layoutModel[] = {
		    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	    };
	    UINT numElements = ARRAYSIZE( layoutModel );
	    HRESULT hr = vertexShader_light.Initialize( device, L"res\\shaders\\Model.fx", layoutModel, numElements );
		COM_ERROR_IF_FAILED( hr, "Failed to create light vertex shader!" );
	    hr = pixelShader_light.Initialize( device, L"res\\shaders\\Model.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create light pixel shader!" );

	    hr = pixelShader_noLight.Initialize( device, L"res\\shaders\\Model_NoLight.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create no light pixel shader!" );

        /*   POST-PROCESSING   */
        D3D11_INPUT_ELEMENT_DESC layoutFull[] = {
		    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    };
        numElements = ARRAYSIZE( layoutFull );
	    hr = vertexShader_full.Initialize( device, L"res\\shaders\\Fullscreen.fx", layoutFull, numElements );
		COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen vertex shader!" );
	    hr = pixelShader_full.Initialize( device, L"res\\shaders\\Fullscreen.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen pixel shader!" );

        /*   SPRITES   */
        D3D11_INPUT_ELEMENT_DESC layoutSprite[] = {
		    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    };
	    numElements = ARRAYSIZE( layoutSprite );
	    hr = vertexShader_2D.Initialize( device, L"res\\shaders\\Sprite.fx", layoutSprite, numElements );
		COM_ERROR_IF_FAILED( hr, "Failed to create 2D vertex shader!" );
	    hr = pixelShader_2D.Initialize( device, L"res\\shaders\\Sprite.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create 2D pixel shader!" );
        hr = pixelShader_2D_discard.Initialize( device, L"res\\shaders\\Sprite_Discard.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create 2D discard pixel shader!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
	
	return true;
}

bool Graphics::InitializeScene()
{
    try
    {
        /*   MODELS   */
        json jFile;
        std::ifstream fileOpen( "res\\objects.json" );
        fileOpen >> jFile;
        std::string version = jFile["version"].get<std::string>();

        json objects = jFile["GameObjects"];
        int size = objects.size();

        for ( unsigned int i = 0; i < size; i++ )
        {
            Drawable drawable;
            json objectDesc = objects.at( i );
            drawable.modelName = objectDesc["Name"];
            drawable.fileName = objectDesc["File"];
            drawable.posX = objectDesc["PosX"];
            drawable.posY = objectDesc["PosY"];
            drawable.posZ = objectDesc["PosZ"];
            drawable.rotX = objectDesc["RotX"];
            drawable.rotY = objectDesc["RotY"];
            drawable.rotZ = objectDesc["RotZ"];
            drawable.scaleX = objectDesc["ScaleX"];
            drawable.scaleY = objectDesc["ScaleY"];
            drawable.scaleZ = objectDesc["ScaleZ"];
            drawables.push_back( drawable );
        }

        for ( unsigned int i = 0; i < drawables.size(); i++ )
        {
            RenderableGameObject model;
            model.SetInitialScale( drawables.at( i ).scaleX, drawables.at( i ).scaleY, drawables.at( i ).scaleZ );
            if ( !model.Initialize( "res\\models\\" + drawables.at( i ).fileName, device.Get(), context.Get(), cb_vs_matrix ) )
                return false;
            model.SetInitialPosition( XMFLOAT3( drawables.at( i ).posX, drawables.at( i ).posY, drawables.at( i ).posZ ) );
            model.SetInitialRotation( XMFLOAT3( drawables.at( i ).rotX, drawables.at( i ).rotY, drawables.at( i ).rotZ ) );
            model.SetModelName( drawables.at( i ).modelName );
            renderables.push_back( model );
        }

        light.SetScale( 1.0f, 1.0f, 1.0f );
		if ( !light.Initialize( device.Get(), context.Get(), cb_vs_matrix ) )
			return false;

        /*   SPRITES   */
        if ( !circle.Initialize( device.Get(), context.Get(), 256, 256, "res\\textures\\circle.png", cb_vs_matrix_2d ) )
            return false;
        circle.SetPosition( XMFLOAT3( windowWidth / 2 - circle.GetWidth() / 2, windowHeight / 2 - circle.GetHeight() / 2, 0 ) );

        if ( !square.Initialize( device.Get(), context.Get(), 256, 256, "res\\textures\\purpleheart.png", cb_vs_matrix_2d ) )
            return false;
        square.SetPosition( XMFLOAT3( windowWidth / 2 - square.GetWidth() / 2, windowHeight / 2 - square.GetHeight() / 2, 0 ) );

        /*   OBJECTS   */
        XMFLOAT2 aspectRatio = { static_cast<float>( windowWidth ), static_cast<float>( windowHeight ) };
        camera2D.SetProjectionValues( aspectRatio.x, aspectRatio.y, 0.0f, 1.0f );

        cameras.emplace( "Main", std::make_shared<Camera3D>() );
        cameras["Main"]->SetInitialPosition( XMFLOAT3( 0.0f, 9.0f, -15.0f ) );
        cameras["Main"]->SetProjectionValues( 70.0f, aspectRatio.x / aspectRatio.y, 0.1f, 1000.0f );

        cameras.emplace( "Sub", std::make_shared<Camera3D>() );
        cameras["Sub"]->SetInitialPosition( XMFLOAT3( 0.0f, 9.0f, -55.0f ) );
        cameras["Sub"]->SetProjectionValues( 70.0f, aspectRatio.x / aspectRatio.y, 0.1f, 1000.0f );

        XMVECTOR lightPosition = cameras["Main"]->GetPositionVector();
		lightPosition += cameras["Main"]->GetForwardVector() + XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		light.SetPosition( lightPosition );
		light.SetRotation( cameras["Main"]->GetRotationFloat3() );    

        /*   VERTEX/INDEX   */
        cube.Initialize( context.Get(), device.Get(), 3 );
        fullscreen.Initialize( context.Get(), device.Get() );
        ground.InitializeInstanced( context.Get(), device.Get(), 400 );

        /*   TEXTURES   */
        HRESULT hr = CreateWICTextureFromFile( device.Get(), L"res\\textures\\CrashBox.png", nullptr, boxTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create box texture from file!" );

        hr = CreateWICTextureFromFile( device.Get(), L"res\\textures\\grass.jpg", nullptr, grassTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create grass texture from file!" );

        /*   CONSTANT BUFFERS   */
        hr = cb_vs_fog.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_vs_fog' Constant Buffer!" );
        cb_vs_fog.data.fogColor = XMFLOAT3( 0.2f, 0.2f, 0.2f );
        cb_vs_fog.data.fogStart = 10.0f;
        cb_vs_fog.data.fogEnd = 50.0f;
        cb_vs_fog.data.fogEnable = false;

        hr = cb_vs_matrix.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_vs_matrix' Constant Buffer!" );

        hr = cb_vs_matrix_2d.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_vs_matrix_2d' Constant Buffer!" );

        hr = cb_vs_fullscreen.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_vs_fullscreen' Constant Buffer!" );

		hr = cb_ps_light.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_ps_light' Constant Buffer!" );

        hr = cb_ps_scene.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_ps_scene' Constant Buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    return true;
}