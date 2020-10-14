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
    static float t = 0.0f;

    if ( gfx.driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += (float) DirectX::XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount64();

        if ( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;

        t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
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
        DirectX::XMMatrixRotationZ( t * 0.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesCube[1],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( 10.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationY( t * 1.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesCube[2],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( 15.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( t * multiplier )
    );

    // pyramid transformations
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesPyramid[0],
        DirectX::XMMatrixScaling( 1.5f, 1.5f, 1.5f ) *
        DirectX::XMMatrixRotationZ( t * 0.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesPyramid[1],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixRotationY( t * 1.5f * multiplier )
    );
    DirectX::XMStoreFloat4x4( &gfx.worldMatricesPyramid[2],
        DirectX::XMMatrixScaling( 0.5f, 0.5f, 0.5f ) *
        DirectX::XMMatrixTranslation( 5.0f, 0.0f, 0.0f ) *
        DirectX::XMMatrixRotationZ( t * multiplier )
    );
}

void Application::Render()
{
	static float clearColor[] = { 0.0f, 0.5f, 0.5f, 1.0f };

	this->gfx.BeginFrame( clearColor );
	this->gfx.RenderFrame();
	this->gfx.EndFrame();
}