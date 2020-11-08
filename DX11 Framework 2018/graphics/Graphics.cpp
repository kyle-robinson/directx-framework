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
#include "../resource.h"
#include <fstream>
#include <map>

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

    for ( int i = 0; i < 3; i++ )
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMStoreFloat4x4( &worldMatrix, DirectX::XMMatrixIdentity() );
        worldMatricesCube.push_back( worldMatrix );
    }

    imgui.Initialize( hWnd, device.Get(), context.Get() );

	return true;
}

void Graphics::BeginFrame()
{
	// clear render target
    renderTarget->BindAsTexture( *this, depthStencil.get(), clearColor );
    depthStencil->ClearDepthStencil( *this );

	// set render state
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    stencilStates["Off"]->Bind( *this );
    blendState->Bind( *this );
    rasterizerSolid == true ? rasterizerStates["Solid"]->Bind( *this ) : rasterizerStates["Wireframe"]->Bind( *this );
    samplerAnisotropic == true ? samplerStates["Anisotropic"]->Bind( *this ) : samplerStates["Point"]->Bind( *this );

    // setup constant buffers
    if ( !cb_vs_fog.ApplyChanges() ) return;
	context->VSSetConstantBuffers( 1, 1, cb_vs_fog.GetAddressOf() );
	context->PSSetConstantBuffers( 1, 1, cb_vs_fog.GetAddressOf() );

    if ( usePointLight )
    {
        light.UpdateConstantBuffer( cb_ps_light );
	    if ( !cb_ps_light.ApplyChanges() ) return;
	    context->PSSetConstantBuffers( 2, 1, cb_ps_light.GetAddressOf() );
    }
    else
    {
        light.UpdateConstantBuffer( cb_ps_lightDirect );
	    if ( !cb_ps_lightDirect.ApplyChanges() ) return;
	    context->PSSetConstantBuffers( 2, 1, cb_ps_lightDirect.GetAddressOf() );
    }

    cb_ps_scene.data.alphaFactor = alphaFactor;
    cb_ps_scene.data.useTexture = useTexture;
    if ( !cb_ps_scene.ApplyChanges() ) return;
	context->PSSetConstantBuffers( 3, 1, cb_ps_scene.GetAddressOf() );
}

void Graphics::RenderFrame()
{
    // setup sprite masking
    if ( useMask ) 
    {
        stencilStates["Mask"]->Bind( *this );
        context->VSSetShader( vertexShader_2D.GetShader(), NULL, 0 );
        context->IASetInputLayout( vertexShader_2D.GetInputLayout() );
        context->PSSetShader( pixelShader_2D_discard.GetShader(), NULL, 0 );
        circleMask == true ? circle.Draw( camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix() ) :
            square.Draw( camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix() );
        stencilStates["Write"]->Bind( *this );
    }
	
    // setup shaders
    if ( usePointLight )
    {
	    context->VSSetShader( vertexShader_light.GetShader(), NULL, 0 );
	    context->IASetInputLayout( vertexShader_light.GetInputLayout() );
        context->PSSetShader( pixelShader_light.GetShader(), NULL, 0 );
    }
    else
    {
        context->VSSetShader( vertexShader_lightDirect.GetShader(), NULL, 0 );
	    context->IASetInputLayout( vertexShader_lightDirect.GetInputLayout() );
        context->PSSetShader( pixelShader_lightDirect.GetShader(), NULL, 0 );
    }

    // render models
    for ( unsigned int i = 0; i < renderables.size(); i++ )
        renderables.at( i ).Draw( camera3D.GetViewMatrix(), camera3D.GetProjectionMatrix() );

    // draw cubes
    UINT offset = 0;
    context->IASetVertexBuffers( 0, 1, vertexBufferCube.GetAddressOf(), vertexBufferCube.StridePtr(), &offset );
    context->IASetIndexBuffer( indexBufferCube.Get(), DXGI_FORMAT_R16_UINT, 0 );
    context->PSSetShaderResources( 0, 1, boxTexture.GetAddressOf() );
    for ( int i = 0; i < worldMatricesCube.size(); i++ )
    {
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4( &worldMatricesCube[i] );
        cb_vs_matrix.data.worldMatrix = worldMatrix;
        if ( !cb_vs_matrix.ApplyChanges() ) return;
        context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
        context->DrawIndexed( indexBufferCube.IndexCount(), 0, 0 );
    }

    //offset = 0;
    //context->IASetVertexBuffers( 0, 1, vertexBufferQuad.GetAddressOf(), vertexBufferQuad.StridePtr(), &offset );
    //context->IASetIndexBuffer( indexBufferQuad.Get(), DXGI_FORMAT_R16_UINT, 0 );
    //context->IASetInputLayout( vertexShader_quad.GetInputLayout() );
    //cb_vs_matrix.data.worldMatrix = XMMatrixIdentity() * XMMatrixScaling( 50.0f, 50.0f, 1.0f )
    //    * XMMatrixRotationX( XMConvertToRadians( 90.0f ) ) * XMMatrixTranslation( 0.0f, 4.8f, 0.0f );
    //if ( !cb_vs_matrix.ApplyChanges() ) return;
    //context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
    //context->PSSetShaderResources( 0, 1, grassTexture.GetAddressOf() );
    //context->DrawIndexed( indexBufferQuad.IndexCount(), 0, 0 );

	context->PSSetShader( pixelShader_noLight.GetShader(), NULL, 0 );
	light.Draw( camera3D.GetViewMatrix(), camera3D.GetProjectionMatrix() );
}

void Graphics::EndFrame()
{
    // set and clear back buffer
    backBuffer->BindAsBuffer( *this, clearColor );

    // render to fullscreen texture
    UINT offset = 0;
    context->PSSetShaderResources( 0, 1, renderTarget->GetShaderResourceViewPtr() );
    context->IASetVertexBuffers( 0, 1, vertexBufferFullscreen.GetAddressOf(), vertexBufferFullscreen.StridePtr(), &offset );
    context->IASetInputLayout( vertexShader_full.GetInputLayout() );
    context->IASetIndexBuffer( indexBufferFullscreen.Get(), DXGI_FORMAT_R16_UINT, 0 );
    context->VSSetShader( vertexShader_full.GetShader(), NULL, 0 );
    context->PSSetShader( pixelShader_full.GetShader(), NULL, 0 );
    cb_vs_fullscreen.data.multiView = multiView;
    if ( !cb_vs_fullscreen.ApplyChanges() ) return;
    context->VSSetConstantBuffers( 0, 1, cb_vs_fullscreen.GetAddressOf() );
    Bind::Rasterizer::DrawSolid( *this, indexBufferFullscreen.IndexCount() ); // always draw as solid

    // display imgui
    imgui.BeginRender();
    imgui.RenderMainWindow( context.Get(), alphaFactor, useTexture, clearColor,
        rasterizerSolid, samplerAnisotropic, multiView, useMask, circleMask );
    imgui.RenderLightWindow( light, cb_ps_light, cb_ps_lightDirect, usePointLight );
    imgui.RenderFogWindow( cb_vs_fog );
    imgui.RenderModelWindow( renderables );
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

    // cube transformations
    DirectX::XMStoreFloat4x4( &worldMatricesCube[0],
        DirectX::XMMatrixScaling( 2.0f, 2.0f, 2.0f ) *
        DirectX::XMMatrixRotationZ( timer * 1.5f ) *
        DirectX::XMMatrixTranslation( 0.0f, 10.0f, 15.0f )
    );
    DirectX::XMStoreFloat4x4( &worldMatricesCube[1],
        DirectX::XMMatrixScaling( 2.0f, 2.0f, 2.0f ) *
        DirectX::XMMatrixTranslation( 10.0f, 10.0f, 0.0f ) *
        DirectX::XMMatrixRotationY( timer )
    );
    DirectX::XMStoreFloat4x4( &worldMatricesCube[2],
        DirectX::XMMatrixScaling( 2.0f, 2.0f, 2.0f ) *
        DirectX::XMMatrixTranslation( 1.0f, 10.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( timer * 1.5f ) *
        DirectX::XMMatrixTranslation( 0.0f, 7.5f, 0.0f )
    );
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
        std::unique_ptr<Bind::Viewport> viewport = std::make_unique<Bind::Viewport>( *this );
        viewport.get()->Bind( *this );

        blendState = std::make_shared<Bind::Blender>( *this );
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

        hr = vertexShader_lightDirect.Initialize( device, L"res\\shaders\\Model_Directional.fx", layoutModel, numElements );
		COM_ERROR_IF_FAILED( hr, "Failed to create directional light vertex shader!" );
	    hr = pixelShader_lightDirect.Initialize( device, L"res\\shaders\\Model_Directional.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create directional light pixel shader!" );

	    hr = pixelShader_noLight.Initialize( device, L"res\\shaders\\Model_NoLight.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create no light pixel shader!" );

        /*   PRIMITIVES   */
        /*D3D11_INPUT_ELEMENT_DESC layoutPrim[] = {
		    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	    };
	    numElements = ARRAYSIZE( layoutPrim );
	    hr = vertexShader_quad.Initialize( device, L"res\\shaders\\Water.fx", layoutPrim, numElements );
		COM_ERROR_IF_FAILED( hr, "Failed to create quad vertex shader!" );
	    hr = pixelShader_quad.Initialize( device, L"res\\shaders\\Water.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create quad pixel shader!" );*/

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
            model.SetScale( drawables.at( i ).scaleX, drawables.at( i ).scaleY, drawables.at( i ).scaleZ );
            if ( !model.Initialize( "res\\models\\" + drawables.at( i ).fileName, device.Get(), context.Get(), cb_vs_matrix ) )
                return false;
            model.SetPosition( DirectX::XMFLOAT3( drawables.at( i ).posX, drawables.at( i ).posY, drawables.at( i ).posZ ) );
            model.SetRotation( DirectX::XMFLOAT3( drawables.at( i ).rotX, drawables.at( i ).rotY, drawables.at( i ).rotZ ) );
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
        camera2D.SetProjectionValues( static_cast<float>( windowWidth ), static_cast<float>( windowHeight ), 0.0f, 1.0f );

        camera3D.SetPosition( XMFLOAT3( 0.0f, 9.0f, -15.0f ) );
	    camera3D.SetProjectionValues( 70.0f, static_cast<float>( windowWidth ) / static_cast<float>( windowHeight ),
		    0.1f, 1000.0f );

        XMVECTOR lightPosition = camera3D.GetPositionVector();
		lightPosition += camera3D.GetForwardVector() + XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		light.SetPosition( lightPosition );
		light.SetRotation( camera3D.GetRotationFloat3() );    

        /*   VERTEX/INDEX   */
        HRESULT hr = vertexBufferCube.Initialize( device.Get(), VTX::verticesCube_PosTexNrm, ARRAYSIZE( VTX::verticesCube_PosTexNrm ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
        hr = indexBufferCube.Initialize( device.Get(), IDX::indicesLightCube, ARRAYSIZE( IDX::indicesLightCube ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

        //hr = vertexBufferQuad.Initialize( device.Get(), VTX::verticesQuad_PosTexNrm, ARRAYSIZE( VTX::verticesQuad_PosTexNrm ) );
        //COM_ERROR_IF_FAILED( hr, "Failed to create quad vertex buffer!" );
        //hr = indexBufferQuad.Initialize( device.Get(), IDX::indicesQuad, ARRAYSIZE( IDX::indicesQuad ) );
        //COM_ERROR_IF_FAILED( hr, "Failed to create quad index buffer!" );

        hr = vertexBufferFullscreen.Initialize( device.Get(), VTX::verticesFullscreen, ARRAYSIZE( VTX::verticesFullscreen ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen quad vertex buffer!" );
        hr = indexBufferFullscreen.Initialize( device.Get(), IDX::indicesFullscreen, ARRAYSIZE( IDX::indicesFullscreen ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen quad index buffer!" );

        /*   TEXTURES   */
        hr = DirectX::CreateWICTextureFromFile( device.Get(), L"res\\textures\\CrashBox.png", nullptr, boxTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create box texture from file!" );

        //hr = DirectX::CreateWICTextureFromFile( device.Get(), L"res\\textures\\grass2.jpg", nullptr, grassTexture.GetAddressOf() );
        //COM_ERROR_IF_FAILED( hr, "Failed to create grass texture from file!" );

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

        hr = cb_ps_lightDirect.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_ps_lightDirect' Constant Buffer!" );

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