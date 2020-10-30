#include "Graphics.h"
#include "../resource.h"

bool Graphics::Initialize( HWND hWnd, int width, int height )
{
	this->windowWidth = width;
	this->windowHeight = height;


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

    ImGui_ImplWin32_Init( hWnd );
    ImGui_ImplDX11_Init( this->device.Get(), this->context.Get() );

	return true;
}

void Graphics::BeginFrame()
{
	// clear render target
    this->context->OMSetRenderTargets( 1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get() );
	this->context->ClearRenderTargetView( this->renderTargetView.Get(), this->clearColor );
	this->context->ClearDepthStencilView( this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

	// set render state
	this->context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	this->context->OMSetDepthStencilState( this->depthStencilState.Get(), 0 );
    this->context->OMSetBlendState( this->blendState.Get(), NULL, 0xFFFFFFFF );
}

void Graphics::RenderFrame()
{
	// setup shaders
	this->context->VSSetShader( this->vertexShader_light.GetShader(), NULL, 0 );
	this->context->IASetInputLayout( this->vertexShader_light.GetInputLayout() );
	this->context->PSSetShader( this->pixelShader_light.GetShader(), NULL, 0 );
    
    // set constant buffers
    this->cb_ps_light.data.dynamicLightColor = light.lightColor;
	this->cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	this->cb_ps_light.data.specularLightColor = light.specularColor;
	this->cb_ps_light.data.specularLightIntensity = light.specularIntensity;
	this->cb_ps_light.data.specularLightPower = light.specularPower;
	this->cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	this->cb_ps_light.data.lightConstant = light.constant;
	this->cb_ps_light.data.lightLinear = light.linear;
	this->cb_ps_light.data.lightQuadratic = light.quadratic;
	this->cb_ps_light.data.useTexture = useTexture;
	this->cb_ps_light.data.alphaFactor = alphaFactor;
	this->cb_ps_light.ApplyChanges();
	this->context->PSSetConstantBuffers( 1, 1, this->cb_ps_light.GetAddressOf() );

    /*   MODELS   */
    this->nanosuit.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );
    
    /*   CUBE   */
    UINT offset = 0;
    this->context->IASetVertexBuffers( 0, 1, this->vertexBufferCube.GetAddressOf(), vertexBufferCube.StridePtr(), &offset );
    this->context->IASetIndexBuffer( this->indexBufferCube.Get(), DXGI_FORMAT_R16_UINT, 0 );
    this->context->PSSetShaderResources( 0, 1, this->boxTexture.GetAddressOf() );
    for ( int i = 0; i < worldMatricesCube.size(); i++ )
    {
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4( &worldMatricesCube[i] );
        this->cb_vs_matrix.data.worldMatrix = worldMatrix;
        this->cb_ps_light.data.useTexture = useTexture;
        if ( !cb_vs_matrix.ApplyChanges() ) return;
        if ( !cb_ps_light.ApplyChanges() ) return;
        this->context->VSSetConstantBuffers( 0, 1, this->cb_vs_matrix.GetAddressOf() );
        this->context->DrawIndexed( this->indexBufferCube.IndexCount(), 0, 0 );
    }

    /*   LIGHT   */
	this->context->PSSetShader( this->pixelShader_noLight.GetShader(), NULL, 0 );
	this->light.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );   
}

void Graphics::EndFrame()
{
    // render to texture
    this->context->OMSetRenderTargets( 1, this->backBuffer.GetAddressOf(), nullptr );
    this->context->ClearRenderTargetView( this->backBuffer.Get(), this->clearColor );

    this->context->RSGetState( this->rasterizerState_Wireframe.GetAddressOf() );

    UINT offset = 0;
    this->context->PSSetShaderResources( 0, 1, this->shaderResourceView.GetAddressOf() );
    this->context->IASetVertexBuffers( 0, 1, this->vertexBufferFullscreen.GetAddressOf(), this->vertexBufferFullscreen.StridePtr(), &offset );
    this->context->IASetInputLayout( this->vertexShader_full.GetInputLayout() );
    this->context->IASetIndexBuffer( this->indexBufferFullscreen.Get(), DXGI_FORMAT_R16_UINT, 0 );
    this->context->VSSetShader( this->vertexShader_full.GetShader(), NULL, 0 );
    this->context->PSSetShader( this->pixelShader_full.GetShader(), NULL, 0 );
    this->context->DrawIndexed( this->indexBufferFullscreen.IndexCount(), 0, 0 );

    // display imgui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    imgui.RenderMainWindow( this->context.Get(), this->clearColor, this->useTexture, this->alphaFactor,
        this->rasterizerState_Solid.Get(), this->rasterizerState_Wireframe.Get(),
        this->samplerState_Anisotropic.Get(), this->samplerState_Point.Get() );
    imgui.RenderLightWindow( this->light, this->cb_ps_light );

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
    
    this->context->OMSetRenderTargets( 1, this->nullRenderTarget.GetAddressOf(), nullptr );
    this->context->PSSetShaderResources( 0, 1, this->nullShaderResourceView.GetAddressOf() );
    
    // display frame
	HRESULT hr = this->swapChain->Present( 1, NULL );
	if ( FAILED( hr ) )
	{
		hr == DXGI_ERROR_DEVICE_REMOVED ?
            ErrorLogger::Log( this->device->GetDeviceRemovedReason(), "Swap Chain. Graphics device removed!" ) :
            ErrorLogger::Log( hr, "Swap Chain failed to render frame!" );
		exit( -1 );
	}
}

void Graphics::Update( float dt )
{
    // model transformations
    this->nanosuit.AdjustRotation( XMFLOAT3( 0.0f, 0.001f * dt, 0.0f ) );

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

bool Graphics::InitializeDirectX( HWND hWnd )
{
    try
    {
        UINT createDeviceFlags = 0;
    #ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE( driverTypes );

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
	    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

        DXGI_SWAP_CHAIN_DESC sd = { 0 };
        sd.BufferCount = 1;
        sd.BufferDesc.Width = windowWidth;
        sd.BufferDesc.Height = windowHeight;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = hWnd;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        HRESULT hr;
        for ( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
        {
            D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDeviceAndSwapChain(
                nullptr,                        // IDXGI Adapter
                driverType,                     // Driver Type
                nullptr,                        // Software Module
                createDeviceFlags,              // Flags for Runtime Layers
                featureLevels,                  // Feature Levels Array
                numFeatureLevels,               // No. of Feature Levels
                D3D11_SDK_VERSION,              // SDK Version
                &sd,                            // Swap Chain Description
                this->swapChain.GetAddressOf(), // Swap Chain Address
                this->device.GetAddressOf(),    // Device Address
                nullptr,                        // Ptr to Feature Level
                this->context.GetAddressOf()    // Context Address
            );
            if ( SUCCEEDED( hr ) )
                break;
        }
        COM_ERROR_IF_FAILED( hr, "Failed to create Device and Swap Chain!" );

        // create a render target view with back buffer
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
        hr = this->swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)pBackBuffer.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Back Buffer!" );
        hr = device->CreateRenderTargetView( pBackBuffer.Get(), nullptr, this->backBuffer.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Render Target View with Back Buffer!" );

        // create texture resource
        D3D11_TEXTURE2D_DESC textureDesc = { 0 };
        textureDesc.Width = this->windowWidth;
        textureDesc.Height = this->windowHeight;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
        hr = this->device->CreateTexture2D( &textureDesc, nullptr, pTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Texture for Render Target!" );

        // create resource view on texture
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        hr = this->device->CreateShaderResourceView( pTexture.Get(), &srvDesc, this->shaderResourceView.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Shader Resource View!" );

        // create the target view on the texture
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = textureDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
        hr = this->device->CreateRenderTargetView( pTexture.Get(), &rtvDesc, this->renderTargetView.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Render Target View with Texture!" );

        // create depth stencil
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            this->windowWidth,
            this->windowHeight );
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        hr = this->device->CreateTexture2D( &depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Depth Stencil Buffer!" );
        hr = this->device->CreateDepthStencilView( this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Depth Stencil View!" );

        // set depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthStencilStateDesc( CD3D11_DEFAULT{} );
		depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = this->device->CreateDepthStencilState( &depthStencilStateDesc, this->depthStencilState.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create Depth Stencil State!" );

        // setup the viewport
        CD3D11_VIEWPORT vp( 0.0f, 0.0f,
            static_cast<FLOAT>( windowWidth ),
            static_cast<FLOAT>( windowHeight ) );
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        this->context->RSSetViewports( 1, &vp );

        // setup rasterizer states
        CD3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
        rasterizerDesc.MultisampleEnable = TRUE;
        hr = this->device->CreateRasterizerState( &rasterizerDesc, this->rasterizerState_Solid.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create solid Rasterizer State!" );

        rasterizerDesc.MultisampleEnable = TRUE;
        rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
        hr = this->device->CreateRasterizerState( &rasterizerDesc, this->rasterizerState_Wireframe.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create wireframe Rasterizer State!" );
        
        this->context->RSSetState( this->rasterizerState_Solid.Get() );

        // set blend state
		D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = { 0 };
		renderTargetBlendDesc.BlendEnable = TRUE;
		renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC blendDesc = { 0 };
		blendDesc.RenderTarget[0] = renderTargetBlendDesc;
		hr = this->device->CreateBlendState( &blendDesc, this->blendState.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create Blend State!" );

        // create sampler states
		CD3D11_SAMPLER_DESC samplerDesc( CD3D11_DEFAULT{} );
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		hr = this->device->CreateSamplerState( &samplerDesc, this->samplerState_Anisotropic.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create anisotropic Sampler State!" );

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.MaxAnisotropy = 1;
		hr = this->device->CreateSamplerState( &samplerDesc, this->samplerState_Point.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create point Sampler State!" );
        
        this->context->PSSetSamplers( 0, 1, this->samplerState_Anisotropic.GetAddressOf() );
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
	    HRESULT hr = vertexShader_light.Initialize( this->device, L"res\\shaders\\Model.fx", layoutModel, numElements );
		COM_ERROR_IF_FAILED( hr, "Failed to create light vertex shader!" );
	    hr = pixelShader_light.Initialize( this->device, L"res\\shaders\\Model.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create light pixel shader!" );
	    hr = pixelShader_noLight.Initialize( this->device, L"res\\shaders\\Model_NoLight.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create no light pixel shader!" );

        /*   POST-PROCESSING   */
        D3D11_INPUT_ELEMENT_DESC layoutFull[] = {
		    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    };
        numElements = ARRAYSIZE( layoutFull );
	    hr = vertexShader_full.Initialize( this->device, L"res\\shaders\\Fullscreen.fx", layoutFull, numElements );
		COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen vertex shader!" );
	    hr = pixelShader_full.Initialize( this->device, L"res\\shaders\\Fullscreen.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen pixel shader!" );
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
        // initialize camera
        camera.SetPosition( XMFLOAT3( 0.0f, 9.0f, -15.0f ) );
	    camera.SetProjectionValues(
		    70.0f,
		    static_cast<float>( this->windowWidth ) / static_cast<float>( this->windowHeight ),
		    0.1f,
		    1000.0f
	    );

		// initialize objects
		if ( !nanosuit.Initialize( "res\\models\\nanosuit\\nanosuit.obj",
			this->device.Get(), this->context.Get(), this->cb_vs_matrix ) )
			return false;

		if ( !light.Initialize( this->device.Get(), this->context.Get(), this->cb_vs_matrix ) )
			return false;

        XMVECTOR lightPosition = this->camera.GetPositionVector();
		lightPosition += this->camera.GetForwardVector() + XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		this->light.SetPosition( lightPosition );
		this->light.SetRotation( this->camera.GetRotationFloat3() );
        
        // cube vertices and indices
        HRESULT hr = this->vertexBufferCube.Initialize( this->device.Get(),
            VTX::verticesCube_PosTexNrm, ARRAYSIZE( VTX::verticesCube_PosTexNrm ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
        hr = this->indexBufferCube.Initialize( this->device.Get(), IDX::indicesLightCube, ARRAYSIZE( IDX::indicesLightCube ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

        // fullscreen texture
        hr = this->vertexBufferFullscreen.Initialize( this->device.Get(),
            VTX::verticesFullscreen, ARRAYSIZE( VTX::verticesFullscreen ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen quad vertex buffer!" );
        hr = this->indexBufferFullscreen.Initialize( this->device.Get(), IDX::indicesFullscreen, ARRAYSIZE( IDX::indicesFullscreen ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen quad index buffer!" );

        // create textures
        hr = DirectX::CreateWICTextureFromFile(
            this->device.Get(),
            L"res\\textures\\CrashBox.png",
            nullptr,
            this->boxTexture.GetAddressOf()
        );
        COM_ERROR_IF_FAILED( hr, "Failed to create WIC texture from file!" );

        // initialize constant buffers
        hr = this->cb_vs_matrix.Initialize( this->device.Get(), this->context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_vs_matrix' Constant Buffer!" );

		hr = this->cb_ps_light.Initialize( this->device.Get(), this->context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_ps_pixelshader' Constant Buffer!" );
		this->cb_ps_light.data.ambientLightColor = XMFLOAT3( 1.0f, 1.0f, 1.0f );
		this->cb_ps_light.data.ambientLightStrength = 0.1f;
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    return true;
}