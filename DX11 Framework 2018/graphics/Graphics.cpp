#include "Graphics.h"
#include "../utility/ErrorLogger.h"
#include "../resource.h"

bool Graphics::Initialize( HWND hWnd, int width, int height )
{
	this->windowWidth = width;
	this->windowHeight = height;

	if ( FAILED( InitializeDirectX( hWnd ) ) )
		return false;

	if ( FAILED( InitializeShaders() ) )
		return false;

	if ( FAILED( InitializeScene() ) )
		return false;

    for ( int i = 0; i < 3; i++ )
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMStoreFloat4x4( &worldMatrix, DirectX::XMMatrixIdentity() );
        worldMatricesCube.push_back( worldMatrix );
        worldMatricesPyramid.push_back( worldMatrix );
    }

    // Initialize the view matrix
	DirectX::XMVECTOR Eye = DirectX::XMVectorSet( 0.0f, 0.0f, -15.0f, 0.0f );
	DirectX::XMVECTOR At  = DirectX::XMVectorSet( 0.0f, 0.0f,  0.0f,  0.0f );
	DirectX::XMVECTOR Up  = DirectX::XMVectorSet( 0.0f, 1.0f,  0.0f,  0.0f );

	DirectX::XMStoreFloat4x4( &view, DirectX::XMMatrixLookAtLH( Eye, At, Up ) );

    // Initialize the projection matrix
	DirectX::XMStoreFloat4x4(
        &projection,
        DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XM_PIDIV2,
            static_cast<FLOAT>( windowWidth ) / static_cast<FLOAT>( windowHeight ),
            0.01f,
            100.0f )
    );

	return true;
}

void Graphics::BeginFrame( float clearColor[4] )
{	
	// clear render target
	this->context->ClearRenderTargetView( this->renderTargetView.Get(), clearColor );
	this->context->ClearDepthStencilView( this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

	// set render state
	this->context->IASetInputLayout( this->vertexShader.GetInputLayout() );
	this->context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	this->context->OMSetDepthStencilState( this->depthStencilState.Get(), 0 );
    this->device->CreateRasterizerState( &rasterizerDesc, this->rasterizerState.GetAddressOf() );
    this->context->RSSetState( this->rasterizerState.Get() );

	// setup shaders
	this->context->VSSetShader( this->vertexShader.GetShader(), NULL, 0 );
	this->context->PSSetShader( this->pixelShader.GetShader(), NULL, 0 );
}

void Graphics::RenderFrame()
{
    /*   CUBE OBJECT   */
    // Setup buffers
    UINT offset = 0;
    this->context->IASetVertexBuffers( 0, 1, this->vertexBufferCube.GetAddressOf(), vertexBufferCube.StridePtr(), &offset );
    this->context->IASetIndexBuffer( this->indexBufferCube.Get(), DXGI_FORMAT_R16_UINT, 0 );

    // Load matrices
	DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4( &reinterpret_cast<DirectX::XMFLOAT4X4&>( view ) );
	DirectX::XMMATRIX projectionMatrix = DirectX::XMLoadFloat4x4( &reinterpret_cast<DirectX::XMFLOAT4X4&>( projection ) );
    for ( int i = 0; i < 3; i++ )
    {
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4( &worldMatricesCube[i] );
        cb_vs_vertexshader.data.mWorld = XMMatrixTranspose( worldMatrix );
        cb_vs_vertexshader.data.mView = XMMatrixTranspose( viewMatrix );
        cb_vs_vertexshader.data.mProjection = XMMatrixTranspose( projectionMatrix );

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

    // Load matrices
    viewMatrix = DirectX::XMLoadFloat4x4( &reinterpret_cast<DirectX::XMFLOAT4X4&>( view ) );
    projectionMatrix = DirectX::XMLoadFloat4x4( &reinterpret_cast<DirectX::XMFLOAT4X4&>( projection ) );
    for ( int i = 0; i < 3; i++ )
    {
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4( &worldMatricesPyramid[i] );
        cb_vs_vertexshader.data.mWorld = XMMatrixTranspose( worldMatrix );
        cb_vs_vertexshader.data.mView = XMMatrixTranspose( viewMatrix );
        cb_vs_vertexshader.data.mProjection = XMMatrixTranspose( projectionMatrix );

        if ( !cb_vs_vertexshader.ApplyChanges() )
		    return;
        this->context->VSSetConstantBuffers( 0, 1, this->cb_vs_vertexshader.GetAddressOf() );
        this->context->DrawIndexed( this->indexBufferPyramid.BufferSize(), 0, 0 );
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

HRESULT Graphics::InitializeDirectX( HWND hWnd )
{
    HRESULT hr = S_OK;
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

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = windowWidth;
    sd.BufferDesc.Height = windowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

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
    if ( FAILED( hr ) )
    {
        ErrorLogger::Log( hr, "Failed to create Device and Swap Chain!" );
        return hr;
    }

    // create depth stencil
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory( &depthStencilDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
    depthStencilDesc.Width = this->windowWidth;
    depthStencilDesc.Height = this->windowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = this->device->CreateTexture2D( &depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf() );
    hr = this->device->CreateDepthStencilView( this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf() );
    if ( FAILED( hr ) )
    {
        ErrorLogger::Log( hr, "Failed to create Depth Stencil View!" );
        return hr;
    }

    // Create a render target view
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = this->swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)pBackBuffer.GetAddressOf() );
    if ( FAILED( hr ) )
    {
        ErrorLogger::Log( hr, "Failed to " );
        return hr;
    }

    hr = device->CreateRenderTargetView( pBackBuffer.Get(), nullptr, this->renderTargetView.GetAddressOf() );
    if ( FAILED( hr ) )
    {
        ErrorLogger::Log( hr, "Failed to create Render Target View!" );
        return hr;
    }

    this->context->OMSetRenderTargets( 1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get() );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>( windowWidth );
    vp.Height = static_cast<FLOAT>( windowHeight );
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    this->context->RSSetViewports( 1, &vp );

    // setup rasterizer state
    ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
    rasterizerDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    hr = this->device->CreateRasterizerState( &rasterizerDesc, this->rasterizerState.GetAddressOf() );
    if ( FAILED( hr ) )
    {
        ErrorLogger::Log( hr, "Failed to create Rasterizer State!" );
        return hr;
    }

    return S_OK;
}

HRESULT Graphics::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE( layout );

	if ( FAILED( vertexShader.Initialize( this->device, L"res\\shaders\\DX11 Framework.fx", layout, numElements ) ) )
		return E_FAIL;

	if ( FAILED( pixelShader.Initialize( this->device, L"res\\shaders\\DX11 Framework.fx" ) ) )
		return E_FAIL;
	
	return S_OK;
}

HRESULT Graphics::InitializeScene()
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
	if ( FAILED( hr ) )
	{
		ErrorLogger::Log( hr, "Failed to create cube vertex buffer!" );
		return hr;
	}

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
	if ( FAILED( hr ) )
	{
		ErrorLogger::Log( hr, "Failed to create cube index buffer!" );
		return hr;
	}

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
	if ( FAILED( hr ) )
	{
		ErrorLogger::Log( hr, "Failed to create pyramid vertex buffer!" );
		return hr;
	}

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
	if ( FAILED( hr ) )
	{
		ErrorLogger::Log( hr, "Failed to create pyramid index buffer!" );
		return hr;
	}

    // setup constant buffers
    hr = this->cb_vs_vertexshader.Initialize( this->device.Get(), this->context.Get() );
	if ( FAILED( hr ) )
	{
		ErrorLogger::Log( hr, "Failed to initialize 'cb_vs_vertexshader' Constant Buffer!" );
		return hr;
	}

    return hr;
}