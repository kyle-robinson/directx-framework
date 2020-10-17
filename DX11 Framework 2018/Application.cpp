#include "Application.h"

bool Application::Initialize(
	HINSTANCE hInstance,
	const std::string& windowTitle,
	const std::string& windowClass,
	int width,
	int height )
{
	if ( !this->renderWindow.Initialize( this, hInstance, windowTitle, windowClass, width, height ) )
		return false;

	if ( !gfx.Initialize( this->renderWindow.GetHWND(), width, height ) )
		return false;

	return true;
}

bool Application::ProcessMessages() noexcept
{
	return this->renderWindow.ProcessMessages();
}

void Application::Update()
{
    static float timer = 0.0f;

    if ( gfx.driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        timer += DirectX::XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount64();

        if ( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;

        timer = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    }

    // Setup render state
    if ( GetAsyncKeyState( VK_SPACE ) )
        gfx.rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    if ( GetAsyncKeyState( VK_LSHIFT ) )
        gfx.rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

    // Animate the cube
    static float multiplier = 1.0f;
    if ( GetAsyncKeyState( VK_UP ) )
        multiplier += 0.1f;
    if ( GetAsyncKeyState( VK_DOWN ) )
        multiplier -= 0.1f;

    // cube transformations
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesCube[0],
        DirectX::XMMatrixScaling( 1.5f, 1.5f, 1.5f ) *
        DirectX::XMMatrixRotationZ( timer * 0.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesCube[1],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( 10.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationY( timer * 1.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesCube[2],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( 15.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( timer * multiplier )
    );
    gfx.gTime = timer;

    // pyramid transformations
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesPyramid[0],
        DirectX::XMMatrixScaling( 1.5f, 1.5f, 1.5f ) *
        DirectX::XMMatrixTranslation( 5.0f, 10.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( timer * 0.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesPyramid[1],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( -10.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationY( timer * 1.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesPyramid[2],
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
            DirectX::XMStoreFloat4x4( &gfx.worldMatricesQuad[count],
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

void Application::Render()
{
	static float clearColor[] = { 0.0f, 0.5f, 0.5f, 1.0f };

	this->gfx.BeginFrame( clearColor );
	this->gfx.RenderFrame();
	this->gfx.EndFrame();
}