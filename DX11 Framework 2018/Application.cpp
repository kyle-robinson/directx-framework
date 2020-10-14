#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBufferCube = nullptr;
	_pIndexBufferCube = nullptr;
    _pVertexBufferPyramid = nullptr;
    _pIndexBufferPyramid = nullptr;
	_pConstantBuffer = nullptr;
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow, int windowWidth, int windowHeight)
{
    this->_WindowWidth = windowWidth;
    this->_WindowHeight = windowHeight;

    if ( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return E_FAIL;

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
        return E_FAIL;

	// Initialize the world matrix
    for (int i = 0; i < 3; i++)
    {
        XMFLOAT4X4 world;
        XMStoreFloat4x4(&world, XMMatrixIdentity());
        _worldMatricesCube.push_back(world);
        _worldMatricesPyramid.push_back(world);
    }

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -15.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT) _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"res\\shaders\\DX11 Framework.fx", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"res\\shaders\\DX11 Framework.fx", "PS", "ps_5_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout.Get());

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
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

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex_Pos_Col) * ARRAYSIZE( verticesCube );
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = verticesCube;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferCube);

    if (FAILED(hr))
        return hr;

    // Create vertex buffer
    Vertex_Pos_Col verticesPyramid[] =
    {
        { { -3.0f, 0.0f,  3.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 3.0f,  0.0f,  3.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { -3.0f, 0.0f, -3.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 3.0f,  0.0f, -3.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { 0.0f,  7.0f,  0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }
    };

	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex_Pos_Col) * ARRAYSIZE( verticesPyramid );
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = verticesPyramid;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferPyramid);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
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

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * ARRAYSIZE( indicesCube );
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indicesCube;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferCube);

    if (FAILED(hr))
        return hr;

    // Create index buffer
    WORD indicesPyramid[] =
    {
        0, 2, 1,    // base
        1, 2, 3,
        0, 1, 4,    // sides
        1, 3, 4,
        3, 2, 4,
        2, 0, 4,
    };

	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * ARRAYSIZE( indicesPyramid );
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indicesPyramid;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferPyramid);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    int centerScreenX = GetSystemMetrics( SM_CXSCREEN ) / 2 - this->_WindowWidth / 2;
    int centerScreenY = GetSystemMetrics( SM_CYSCREEN ) / 2 - this->_WindowHeight / 2;

    RECT rc = { 0 };
    rc.left = centerScreenX;
    rc.top = centerScreenY;
    rc.right = rc.left + this->_WindowWidth;
    rc.bottom = rc.top + this->_WindowHeight;
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

    _hInst = hInstance;
    _hWnd = CreateWindow(
        L"TutorialWindowClass",
        L"DX11 Framework",
        WS_OVERLAPPEDWINDOW,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    if (!_hWnd)
		return E_FAIL;

    ShowWindow( _hWnd, nCmdShow );
    SetForegroundWindow( _hWnd );
    SetFocus( _hWnd );

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
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

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            _driverType,
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &sd,
            _pSwapChain.GetAddressOf(),
            _pd3dDevice.GetAddressOf(),
            &_featureLevel,
            _pImmediateContext.GetAddressOf()
        );
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // create depth stencil
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory( &depthStencilDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
    depthStencilDesc.Width = this->_WindowWidth;
    depthStencilDesc.Height = this->_WindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    this->_pd3dDevice->CreateTexture2D( &depthStencilDesc, NULL, &_depthStencilBuffer );
    this->_pd3dDevice->CreateDepthStencilView( _depthStencilBuffer.Get(), NULL, _depthStencilView.GetAddressOf() );

    // Create a render target view
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, _pRenderTargetView.GetAddressOf());
    //pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    _pImmediateContext->OMSetRenderTargets(1, _pRenderTargetView.GetAddressOf(), _depthStencilView.Get());

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

    // setup rasterizer state
    ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
    rasterizerDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    hr = this->_pd3dDevice->CreateRasterizerState( &rasterizerDesc, _rasterizerState.GetAddressOf() );

	InitShadersAndInputLayout();

    InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_VS_vertexshader);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, _pConstantBuffer.GetAddressOf());

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount64();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

    // Animate the cube
    static float multiplier = 1.0f;
    if ( GetAsyncKeyState( VK_UP ) )
        multiplier += 0.1f;
    if ( GetAsyncKeyState( VK_DOWN ) )
        multiplier -= 0.1f;

    XMStoreFloat4x4(&_worldMatricesCube[0], XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMMatrixRotationZ(t * 0.5f * multiplier));
    XMStoreFloat4x4(&_worldMatricesCube[1], XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(10.0f, 0.0f, 0.0f) * XMMatrixRotationY(t * 1.5f * multiplier));
    XMStoreFloat4x4(&_worldMatricesCube[2], XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(15.0f, 0.0f, 0.0f) * XMMatrixRotationZ(t * multiplier));

    XMStoreFloat4x4(&_worldMatricesPyramid[0], XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMMatrixRotationZ(t * 0.5f * multiplier));
    XMStoreFloat4x4(&_worldMatricesPyramid[1], XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(t * 1.5f * multiplier));
    XMStoreFloat4x4(&_worldMatricesPyramid[2], XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(5.0f, 0.0f, 0.0f) * XMMatrixRotationZ(t * multiplier));
}

void Application::Draw()
{
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView.Get(), ClearColor);
    _pImmediateContext->ClearDepthStencilView( _depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    // Setup render state
    if (GetAsyncKeyState(VK_SPACE))
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    if (GetAsyncKeyState(VK_LSHIFT))
        rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

    /*   CUBE OBJECT   */
    // Setup buffers
    UINT stride = sizeof(Vertex_Pos_Col);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, _pVertexBufferCube.GetAddressOf(), &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(_pIndexBufferCube.Get(), DXGI_FORMAT_R16_UINT, 0);

    _pd3dDevice->CreateRasterizerState(&rasterizerDesc, _rasterizerState.GetAddressOf());
    _pImmediateContext->RSSetState(_rasterizerState.Get());

    // Setup shaders
    _pImmediateContext->VSSetShader(_pVertexShader.Get(), nullptr, 0);
    _pImmediateContext->VSSetConstantBuffers(0, 1, _pConstantBuffer.GetAddressOf());
    _pImmediateContext->PSSetConstantBuffers(0, 1, _pConstantBuffer.GetAddressOf());
    _pImmediateContext->PSSetShader(_pPixelShader.Get(), nullptr, 0);

    // Matrices
    CB_VS_vertexshader cb;
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
    for ( int i = 0; i < 3; i++ )
    {
        XMMATRIX world = XMLoadFloat4x4(&_worldMatricesCube[i]);
        cb.mWorld = XMMatrixTranspose(world);
        cb.mView = XMMatrixTranspose(view);
        cb.mProjection = XMMatrixTranspose(projection);

        _pImmediateContext->UpdateSubresource(_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
        _pImmediateContext->DrawIndexed(36, 0, 0);
    }

    /*   PYRAMID OBJECT   */
    // Setup buffers
    offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, _pVertexBufferPyramid.GetAddressOf(), &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(_pIndexBufferPyramid.Get(), DXGI_FORMAT_R16_UINT, 0);

    // Setup shaders
    _pImmediateContext->VSSetConstantBuffers(0, 1, _pConstantBuffer.GetAddressOf());
    _pImmediateContext->PSSetConstantBuffers(0, 1, _pConstantBuffer.GetAddressOf());

    // Matrices
    CB_VS_vertexshader cb2;
    view = XMLoadFloat4x4(&_view);
    projection = XMLoadFloat4x4(&_projection);
    for ( int i = 0; i < 3; i++ )
    {
        XMMATRIX world = XMLoadFloat4x4(&_worldMatricesPyramid[i]);
        cb2.mWorld = XMMatrixTranspose(world);
        cb2.mView = XMMatrixTranspose(view);
        cb2.mProjection = XMMatrixTranspose(projection);

        _pImmediateContext->UpdateSubresource(_pConstantBuffer.Get(), 0, nullptr, &cb2, 0, 0);
        _pImmediateContext->DrawIndexed(18, 0, 0);
    }

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);
}