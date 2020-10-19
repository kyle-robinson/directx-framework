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
        worldMatricesPyramid.push_back( worldMatrix );
    }
    for ( int i = 0; i < 1000; i++ )
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMStoreFloat4x4( &worldMatrix, DirectX::XMMatrixIdentity() );
        worldMatricesQuad.push_back( worldMatrix );
    }

	return true;
}

void Graphics::BeginFrame( float clearColor[4] )
{	
	// clear render target
	this->context->ClearRenderTargetView( this->renderTargetView.Get(), clearColor );
	this->context->ClearDepthStencilView( this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

	// set render state
	this->context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	this->context->OMSetDepthStencilState( this->depthStencilState.Get(), 0 );
    this->device->CreateRasterizerState( &rasterizerDesc, this->rasterizerState.GetAddressOf() );
    this->context->RSSetState( this->rasterizerState.Get() );

	// setup shaders
	this->context->VSSetShader( this->vertexShader.GetShader(), NULL, 0 );
	this->context->IASetInputLayout( this->vertexShader.GetInputLayout() );
	this->context->PSSetShader( this->pixelShader.GetShader(), NULL, 0 );
}

void Graphics::RenderFrame()
{
    cb_vs_vertexshader.data.gTime = gTime;

    // Load matrices
    cb_vs_vertexshader.data.mView = camera.GetViewMatrix();
    cb_vs_vertexshader.data.mProjection = camera.GetProjectionMatrix();

    /*   CUBE OBJECT   */
    // Setup buffers
    UINT offset = 0;
    this->context->IASetVertexBuffers( 0, 1, this->vertexBufferCube.GetAddressOf(), vertexBufferCube.StridePtr(), &offset );
    this->context->IASetIndexBuffer( this->indexBufferCube.Get(), DXGI_FORMAT_R16_UINT, 0 );
    for ( int i = 0; i < worldMatricesCube.size(); i++ )
    {
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4( &worldMatricesCube[i] );
        cb_vs_vertexshader.data.mWorld = DirectX::XMMatrixTranspose( worldMatrix );

        if ( !cb_vs_vertexshader.ApplyChanges() )
		    return;
        this->context->VSSetConstantBuffers( 0, 1, this->cb_vs_vertexshader.GetAddressOf() );
        this->context->DrawIndexed( this->indexBufferCube.BufferSize(), 0, 0 );
    }

    /*   PYRAMID OBJECT   */
    // Setup buffers
    offset = 0;
    this->context->IASetVertexBuffers( 0, 1, this->vertexBufferPyramid.GetAddressOf(), vertexBufferPyramid.StridePtr(), &offset );
    this->context->IASetIndexBuffer( this->indexBufferPyramid.Get(), DXGI_FORMAT_R16_UINT, 0 );
    for ( int i = 0; i < worldMatricesPyramid.size(); i++ )
    {
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4( &worldMatricesPyramid[i] );
        cb_vs_vertexshader.data.mWorld = DirectX::XMMatrixTranspose( worldMatrix );

        if ( !cb_vs_vertexshader.ApplyChanges() )
		    return;
        this->context->VSSetConstantBuffers( 0, 1, this->cb_vs_vertexshader.GetAddressOf() );
        this->context->DrawIndexed( this->indexBufferPyramid.BufferSize(), 0, 0 );
    }

    /*   QUAD OBJECT   */
    // Setup buffers
    offset = 0;
    this->context->IASetVertexBuffers( 0, 1, this->vertexBufferQuad.GetAddressOf(), vertexBufferQuad.StridePtr(), &offset );
    this->context->IASetIndexBuffer( this->indexBufferQuad.Get(), DXGI_FORMAT_R16_UINT, 0 );
    for ( int i = 0; i < worldMatricesQuad.size(); i++ )
    {
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4( &worldMatricesQuad[i] );
        cb_vs_vertexshader.data.mWorld = DirectX::XMMatrixTranspose( worldMatrix );

        if ( !cb_vs_vertexshader.ApplyChanges() )
		    return;
        this->context->VSSetConstantBuffers( 0, 1, this->cb_vs_vertexshader.GetAddressOf() );
        this->context->DrawIndexed( this->indexBufferQuad.BufferSize(), 0, 0 );
    }
}

void Graphics::EndFrame()
{
	// display frame
	HRESULT hr = this->swapChain->Present( 1, NULL );
	if ( FAILED( hr ) )
	{
		if ( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			ErrorLogger::Log( this->device->GetDeviceRemovedReason(), "Swap Chain. Graphics device removed!" );
			exit( -1 );
		}
		else
		{
			ErrorLogger::Log( hr, "Swap Chain failed to render frame!" );
			exit( -1 );
		}
	}
}

void Graphics::Update()
{
    // setup timer
	static float timer = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount64();

	if ( dwTimeStart == 0 )
		dwTimeStart = dwTimeCur;

	timer = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    gTime = timer;

    // cube transformations
    DirectX::XMStoreFloat4x4( &worldMatricesCube[0],
        DirectX::XMMatrixScaling( 1.5f, 1.5f, 1.5f ) *
        DirectX::XMMatrixRotationZ( timer * 0.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &worldMatricesCube[1],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( 10.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationY( timer * 1.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &worldMatricesCube[2],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( 15.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( timer * multiplier )
    );

    // pyramid transformations
    DirectX::XMStoreFloat4x4( &worldMatricesPyramid[0],
        DirectX::XMMatrixScaling( 1.5f, 1.5f, 1.5f ) *
        DirectX::XMMatrixTranslation( 5.0f, 10.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( timer * 0.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &worldMatricesPyramid[1],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( -10.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationY( timer * 1.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &worldMatricesPyramid[2],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( -10.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( timer * multiplier )
    );

    // quad transformations
    int count = 0;
    static int offset = 6;
    static int widthLimit = 20;
    static int heightLimit = 30;
    for ( int width = 0; width < 20; width++ )
    {
        for ( int height = 0; height < 20; height++ )
        {
            DirectX::XMStoreFloat4x4( &worldMatricesQuad[count],
                DirectX::XMMatrixRotationX( 1.5708f ) *
                DirectX::XMMatrixTranslation(
                    ( width * offset ) - ( widthLimit + heightLimit ),
                    -20.0f,
                    ( height * offset ) - heightLimit
                )
            );
            count++;
        }
    }
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
        sd.SampleDesc.Count = 4;
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
            driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDeviceAndSwapChain(
                nullptr,
                this->driverType,
                nullptr,
                createDeviceFlags,
                featureLevels,
                numFeatureLevels,
                D3D11_SDK_VERSION,
                &sd,
                this->swapChain.GetAddressOf(),
                this->device.GetAddressOf(),
                &this->featureLevel,
                this->context.GetAddressOf()
            );
            if ( SUCCEEDED( hr ) )
                break;
        }
        COM_ERROR_IF_FAILED( hr, "Failed to create Device and Swap Chain!" );

        // create a render target view
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
        hr = this->swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)pBackBuffer.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Back Buffer!" );
        hr = device->CreateRenderTargetView( pBackBuffer.Get(), nullptr, this->renderTargetView.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Render Target View!" );

        // create depth stencil
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            this->windowWidth,
            this->windowHeight );
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.SampleDesc.Count = 4;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        hr = this->device->CreateTexture2D( &depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Depth Stencil Buffer!" );
        hr = this->device->CreateDepthStencilView( this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Depth Stencil View!" );
        this->context->OMSetRenderTargets( 1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get() );

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

        // setup rasterizer state
        rasterizerDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
        rasterizerDesc.MultisampleEnable = TRUE;
        hr = this->device->CreateRasterizerState( &rasterizerDesc, this->rasterizerState.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create Rasterizer State!" );
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
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE( layout );

    try
    {
	    HRESULT hr = vertexShader.Initialize( this->device, L"res\\shaders\\DX11 Framework.fx", layout, numElements );
        COM_ERROR_IF_FAILED( hr, "Failed to initialize Vertex Shader!" );

	    hr = pixelShader.Initialize( this->device, L"res\\shaders\\DX11 Framework.fx" );
        COM_ERROR_IF_FAILED( hr, "Failed to create Pixel Shader!" );
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
        // cube vertices and indices
        Vertex_Pos_Col verticesCube[] =
        {
            { { -3.0f,  3.0f, -3.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
            { {  3.0f,  3.0f, -3.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
            { { -3.0f, -3.0f, -3.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
            { {  3.0f, -3.0f, -3.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { -3.0f,  3.0f,  3.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
            { {  3.0f,  3.0f,  3.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -3.0f, -3.0f,  3.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
            { {  3.0f, -3.0f,  3.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }
        };
        HRESULT hr = this->vertexBufferCube.Initialize( this->device.Get(), verticesCube, ARRAYSIZE( verticesCube ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );

        WORD indicesCube[] =
        {
            0, 1, 2,    // side 1
            2, 1, 3,
            4, 0, 6,    // side 2
            6, 0, 2,
            7, 5, 6,    // side 3
            6, 5, 4,
            3, 1, 7,    // side 4
            7, 1, 5,
            4, 5, 0,    // side 5
            0, 5, 1,
            3, 7, 2,    // side 6
            2, 7, 6
        };
        hr = this->indexBufferCube.Initialize( this->device.Get(), indicesCube, ARRAYSIZE( indicesCube ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

        // pyramid vertices and indices
        Vertex_Pos_Col verticesPyramid[] =
        {
            { { -3.0f, 0.0f,  3.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { 3.0f,  0.0f,  3.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
            { { -3.0f, 0.0f, -3.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 3.0f,  0.0f, -3.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
            { { 0.0f,  7.0f,  0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }
        };
        hr = this->vertexBufferPyramid.Initialize( this->device.Get(), verticesPyramid, ARRAYSIZE( verticesPyramid ) );
	    COM_ERROR_IF_FAILED( hr, "Failed to create pyramid vertex buffer" );

        WORD indicesPyramid[] =
        {
            0, 2, 1,    // base
            1, 2, 3,
            0, 1, 4,    // sides
            1, 3, 4,
            3, 2, 4,
            2, 0, 4,
        };
        hr = this->indexBufferPyramid.Initialize( this->device.Get(), indicesPyramid, ARRAYSIZE( indicesPyramid ) );
	    COM_ERROR_IF_FAILED( hr, "Failed to create pyramid index buffer!" );

        // quad vertices and indices
        Vertex_Pos_Col verticesQuad[] =
        {
            { { -3.0f,  3.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
            { {  3.0f,  3.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -3.0f, -3.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { {  3.0f, -3.0f, 0.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } }
        };
        hr = this->vertexBufferQuad.Initialize( this->device.Get(), verticesQuad, ARRAYSIZE( verticesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad vertex buffer!" );

        WORD indicesQuad[] =
        {
            0, 1, 2,
            1, 3, 2
        };
        hr = this->indexBufferQuad.Initialize( this->device.Get(), indicesQuad, ARRAYSIZE( indicesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad index buffer!" );

        // setup constant buffers
        hr = this->cb_vs_vertexshader.Initialize( this->device.Get(), this->context.Get() );
	    COM_ERROR_IF_FAILED( hr, "Failed to initialize 'cb_vs_vertexshader' Constant Buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }

    // initialize camera
    camera.SetPosition( XMFLOAT3( 0.0f, 0.0f, -25.0f ) );
	camera.SetProjectionValues(
		70.0f,
		static_cast<float>( this->windowWidth ) / static_cast<float>( this->windowHeight ),
		0.1f,
		1000.0f
	);

    return true;
}