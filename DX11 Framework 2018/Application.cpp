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
				gfx.camera3D.AdjustRotation(
					XMFLOAT3(
						static_cast<float>( me.GetPosY() ) * 0.005f,
						static_cast<float>( me.GetPosX() ) * 0.005f,
						0.0f
					)
				);
			}
		}
	}

	gfx.camera3D.SetCameraSpeed( 0.002f );

	if ( keyboard.KeyIsPressed( VK_SHIFT ) )
		gfx.camera3D.UpdateCameraSpeed( 0.01f );

	if ( keyboard.KeyIsPressed( 'W' ) )
		gfx.camera3D.AdjustPosition( gfx.camera3D.GetForwardVector() * gfx.camera3D.GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( 'A' ) )
		gfx.camera3D.AdjustPosition( gfx.camera3D.GetLeftVector() * gfx.camera3D.GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( 'S' ) )
		gfx.camera3D.AdjustPosition( gfx.camera3D.GetBackwardVector() * gfx.camera3D.GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( 'D' ) )
		gfx.camera3D.AdjustPosition( gfx.camera3D.GetRightVector() * gfx.camera3D.GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( VK_SPACE ) )
		gfx.camera3D.AdjustPosition( XMFLOAT3( 0.0f, gfx.camera3D.GetCameraSpeed() * dt, 0.0f ) );

	if ( keyboard.KeyIsPressed( 'E' ) )
		gfx.camera3D.AdjustPosition( XMFLOAT3( 0.0f, -gfx.camera3D.GetCameraSpeed() * dt, 0.0f ) );

	if ( keyboard.KeyIsPressed( 'C' ) )
	{
		XMVECTOR lightPosition = gfx.camera3D.GetPositionVector();
		lightPosition += gfx.camera3D.GetForwardVector();
		gfx.light.SetPosition( lightPosition );
		gfx.light.SetRotation( gfx.camera3D.GetRotationFloat3() );
	}

    gfx.Update( dt );
}

void Application::Render()
{
	gfx.BeginFrame();
	gfx.RenderFrame();
	gfx.EndFrame();
}