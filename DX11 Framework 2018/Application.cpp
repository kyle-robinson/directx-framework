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

    // read input
    while ( !keyboard.CharBufferIsEmpty() )
	{
		unsigned char ch = keyboard.ReadChar();
	}
	while ( !keyboard.KeyBufferIsEmpty() )
	{
		Keyboard::KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while ( !mouse.EventBufferIsEmpty() )
	{
		Mouse::MouseEvent me = mouse.ReadEvent();
        if ( mouse.IsRightDown() )
		{
			if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove )
			{
				this->gfx.camera.AdjustRotation(
					XMFLOAT3(
						static_cast<float>( me.GetPosY() ) * 0.005f,
						static_cast<float>( me.GetPosX() ) * 0.005f,
						0.0f
					)
				);
			}
		}
	}

    // camera input
    const float cameraSpeed = 0.02f;
	if ( keyboard.KeyIsPressed( 'W' ) )
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetForwardVector() * cameraSpeed * timer );

	if ( keyboard.KeyIsPressed( 'A' ) )
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetLeftVector() * cameraSpeed * timer );

	if ( keyboard.KeyIsPressed( 'S' ) )
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetBackwardVector() * cameraSpeed * timer );

	if ( keyboard.KeyIsPressed( 'D' ) )
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetRightVector() * cameraSpeed * timer );

	if ( keyboard.KeyIsPressed( VK_SPACE ) )
		this->gfx.camera.AdjustPosition( XMFLOAT3( 0.0f, cameraSpeed * timer, 0.0f ) );

	if ( keyboard.KeyIsPressed( VK_SHIFT ) )
		this->gfx.camera.AdjustPosition( XMFLOAT3( 0.0f, -cameraSpeed * timer, 0.0f ) );

    // Setup render state
    if ( keyboard.KeyIsPressed( VK_F1 ) )
        gfx.rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    if ( keyboard.KeyIsPressed( VK_F2 ) )
        gfx.rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

    // Animate the cube
    static float multiplier = 1.0f;
    if ( keyboard.KeyIsPressed( VK_UP ) )
        multiplier += 0.1f;
    if ( keyboard.KeyIsPressed( VK_DOWN ) )
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