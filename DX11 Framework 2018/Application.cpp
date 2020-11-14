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
				gfx.cameras[gfx.cameraToUse]->AdjustRotation(
					XMFLOAT3(
						static_cast<float>( me.GetPosY() ) * 0.005f,
						static_cast<float>( me.GetPosX() ) * 0.005f,
						0.0f
					)
				);
			}
		}
	}

	// camera to use
	if ( keyboard.KeyIsPressed( '1' ) )
		gfx.cameraToUse = "Main";
	
	if ( keyboard.KeyIsPressed( '2' ) )
		gfx.cameraToUse = "Point";

	if ( keyboard.KeyIsPressed( '3' ) )
		gfx.cameraToUse = "Third";

	// camera speed
	gfx.cameras[gfx.cameraToUse]->SetCameraSpeed( 0.002f );

	if ( keyboard.KeyIsPressed( VK_SHIFT ) )
		gfx.cameras[gfx.cameraToUse]->UpdateCameraSpeed( 0.01f );

	if ( gfx.sceneParams.cameraCollision )
		gfx.cameras[gfx.cameraToUse]->AdjustPosition( gfx.cameras[gfx.cameraToUse]->GetBackwardVector() *
			gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt );

	// camera movement
	if ( keyboard.KeyIsPressed( 'W' ) )
		gfx.cameras[gfx.cameraToUse]->AdjustPosition( gfx.cameras[gfx.cameraToUse]->GetForwardVector() *
			gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( 'A' ) )
		gfx.cameras[gfx.cameraToUse]->AdjustPosition( gfx.cameras[gfx.cameraToUse]->GetLeftVector() *
			gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( 'S' ) )
		gfx.cameras[gfx.cameraToUse]->AdjustPosition( gfx.cameras[gfx.cameraToUse]->GetBackwardVector() *
			gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( 'D' ) )
		gfx.cameras[gfx.cameraToUse]->AdjustPosition( gfx.cameras[gfx.cameraToUse]->GetRightVector() *
			gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt );

	if ( keyboard.KeyIsPressed( VK_SPACE ) )
		gfx.cameras[gfx.cameraToUse]->AdjustPosition( XMFLOAT3( 0.0f, gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt, 0.0f ) );

	if ( keyboard.KeyIsPressed( 'E' ) )
		gfx.cameras[gfx.cameraToUse]->AdjustPosition( XMFLOAT3( 0.0f, -gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt, 0.0f ) );

	// light object position
	XMVECTOR lightPosition = gfx.cameras[gfx.cameraToUse]->GetPositionVector();
	lightPosition += gfx.cameras[gfx.cameraToUse]->GetForwardVector() / 2;
	if ( keyboard.KeyIsPressed( 'C' ) || lightStuck )
	{
		lightStuck = true;
		lightPosition += gfx.cameras[gfx.cameraToUse]->GetRightVector() / 2;
		lightPosition -= { 0.0f, 0.25f, 0.0f, 0.0f };
		gfx.light.SetPosition( lightPosition );
		gfx.light.SetRotation( gfx.cameras[gfx.cameraToUse]->GetRotationVector() );
	}
	if ( keyboard.KeyIsPressed( 'X' ) && lightStuck )
	{
		lightStuck = false;
		gfx.light.SetPosition( lightPosition );
	}

	// manage viewports
	if ( keyboard.KeyIsPressed( VK_UP ) )
	{
		gfx.viewportParams = { 0 };
		gfx.viewportParams.useFull = true;
	}

	if ( keyboard.KeyIsPressed( VK_DOWN ) )
	{
		gfx.viewportParams = { 0 };
		gfx.viewportParams.useSplit = true;
	}

	if ( keyboard.KeyIsPressed( VK_LEFT ) )
	{
		gfx.viewportParams = { 0 };
		gfx.viewportParams.useLeft = true;
	}
	
	if ( keyboard.KeyIsPressed( VK_RIGHT ) )
	{
		gfx.viewportParams = { 0 };
		gfx.viewportParams.useRight = true;
	}

    gfx.Update( dt );
}

void Application::Render()
{
	if ( !gfx.viewportParams.useSplit )
	{
		gfx.BeginFrame();
		gfx.RenderFrame();
		gfx.EndFrame();
	}
	else
	{
		gfx.viewportParams.useLeft = true;
		gfx.BeginFrame();
		gfx.RenderFrame();
		gfx.EndFrame();

		gfx.viewportParams.useRight = true;
		gfx.BeginFrame();
		gfx.RenderFrame();
		gfx.EndFrame();
	}
}