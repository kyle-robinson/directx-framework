#include "Application.h"
#include "imgui/imgui.h"

bool Application::Initialize(
	HINSTANCE hInstance,
	const std::string& windowTitle,
	const std::string& windowClass,
	int width,
	int height )
{
	timer.Start();

	if ( !renderWindow.Initialize( this, hInstance, windowTitle, windowClass, width, height ) )
		return false;

	if ( !gfx.Initialize( renderWindow.GetHWND(), width, height ) )
		return false;

	return true;
}

bool Application::ProcessMessages() noexcept
{
	return renderWindow.ProcessMessages();
}

void Application::Update()
{
    float dt = timer.GetMilliSecondsElapsed();
	timer.Restart();

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
				gfx.camera.AdjustRotation(
					XMFLOAT3(
						static_cast<float>( me.GetPosY() ) * 0.005f,
						static_cast<float>( me.GetPosX() ) * 0.005f,
						0.0f
					)
				);
			}
		}
	}

	float cameraSpeed = 0.002f;

	if ( keyboard.KeyIsPressed( VK_SHIFT ) )
		cameraSpeed = 0.01f;

	if ( keyboard.KeyIsPressed( 'W' ) )
		gfx.camera.AdjustPosition( gfx.camera.GetForwardVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( 'A' ) )
		gfx.camera.AdjustPosition( gfx.camera.GetLeftVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( 'S' ) )
		gfx.camera.AdjustPosition( gfx.camera.GetBackwardVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( 'D' ) )
		gfx.camera.AdjustPosition( gfx.camera.GetRightVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( VK_SPACE ) )
		gfx.camera.AdjustPosition( XMFLOAT3( 0.0f, cameraSpeed * dt, 0.0f ) );

	if ( keyboard.KeyIsPressed( 'E' ) )
		gfx.camera.AdjustPosition( XMFLOAT3( 0.0f, -cameraSpeed * dt, 0.0f ) );

	if ( keyboard.KeyIsPressed( 'C' ) )
	{
		XMVECTOR lightPosition = gfx.camera.GetPositionVector();
		lightPosition += gfx.camera.GetForwardVector();
		gfx.light.SetPosition( lightPosition );
		gfx.light.SetRotation( gfx.camera.GetRotationFloat3() );
	}

    gfx.Update( dt );
}

void Application::Render()
{
	gfx.BeginFrame();
	gfx.RenderFrame();
	gfx.EndFrame();
}