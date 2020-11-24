#include "Application.h"
#include "imgui/imgui.h"
#include "utility/Structs.h"
#include "graphics/CameraMove.h"

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
		std::string outMsg = "X: ";
		outMsg += std::to_string( me.GetPosX() );
		outMsg += ", Y: ";
		outMsg += std::to_string( me.GetPosY() );
		outMsg += "\n";
		OutputDebugStringA( outMsg.c_str() );
        if ( mouse.IsRightDown() )
		{
			if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove && gfx.gameState != Graphics::GameState::MENU )
			{
				if ( gfx.cameraToUse != "Third" )
				{
					gfx.cameras[gfx.cameraToUse]->AdjustRotation(
						static_cast<float>( me.GetPosY() ) * 0.005f,
						static_cast<float>( me.GetPosX() ) * 0.005f,
						0.0f
					);
				}
				else
				{
					gfx.renderables[0].AdjustRotation(
						0.0f,
						static_cast<float>( me.GetPosX() ) * 0.005f,
						0.0f
					);
				}
			}
		}
		if( me.GetType() == Mouse::MouseEvent::EventType::LPress && gfx.gameState != Graphics::GameState::MENU )
		{
			gfx.mousePick.UpdateMatrices( gfx.cameras["Main"]->GetViewMatrix(), gfx.cameras["Main"]->GetProjectionMatrix() );
			if ( gfx.mousePick.TestIntersection( me.GetPosX(), me.GetPosY(), gfx.light ) )
				lightParams.lightIntersection = true;
		}
		if( me.GetType() == Mouse::MouseEvent::EventType::Move && gfx.gameState != Graphics::GameState::MENU )
		{
			gfx.mousePick.UpdateMatrices( gfx.cameras["Main"]->GetViewMatrix(), gfx.cameras["Main"]->GetProjectionMatrix() );
			if ( gfx.mousePick.TestIntersection( me.GetPosX(), me.GetPosY(), gfx.light ) )
				lightParams.lightHover = true;
		}
		else
			lightParams.lightHover = false;
	}

	// manage menu system
	if ( keyboard.KeyIsPressed( VK_RETURN ) && gfx.gameState == Graphics::GameState::MENU )
		gfx.gameState = Graphics::GameState::PLAY;

	// camera to use
	if ( gfx.gameState != Graphics::GameState::MENU )
	{
		// set game state
		if ( keyboard.KeyIsPressed( VK_F1 ) && gfx.gameState != Graphics::GameState::MENU )
			gfx.gameState = Graphics::GameState::PLAY;

		if ( keyboard.KeyIsPressed( VK_F2 ) && gfx.gameState != Graphics::GameState::MENU )
			gfx.gameState = Graphics::GameState::EDIT;

		// set camera mode
		if ( gfx.gameState == Graphics::GameState::PLAY )
			gfx.flyCamera = false;
		
		// set camera to use
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

		if ( gfx.cameraToUse != "Third" )
		{
			// camera collision
			if ( sceneParams.cameraCollision && gfx.cameraToUse == "Third" )
				gfx.cameras[gfx.cameraToUse]->AdjustPosition( gfx.cameras[gfx.cameraToUse]->GetBackwardVector() *
					gfx.cameras[gfx.cameraToUse]->GetCameraSpeed() * dt );

			// camera movement
			if ( keyboard.KeyIsPressed( 'W' ) )
				CameraMove::MoveForward( gfx.cameras[gfx.cameraToUse], dt );

			if ( keyboard.KeyIsPressed( 'A' ) )
				CameraMove::MoveLeft( gfx.cameras[gfx.cameraToUse], dt );

			if ( keyboard.KeyIsPressed( 'S' ) )
				CameraMove::MoveBackward( gfx.cameras[gfx.cameraToUse], dt );

			if ( keyboard.KeyIsPressed( 'D' ) )
				CameraMove::MoveRight( gfx.cameras[gfx.cameraToUse], dt );

			if ( keyboard.KeyIsPressed( VK_SPACE ) )
				CameraMove::MoveUp( gfx.cameras[gfx.cameraToUse], dt );

			if ( keyboard.KeyIsPressed( 'E' ) )
				CameraMove::MoveDown( gfx.cameras[gfx.cameraToUse], dt );

			// static camera
			if ( !gfx.flyCamera && gfx.cameraToUse == "Main" )
				gfx.cameras[gfx.cameraToUse]->SetPosition( gfx.cameras[gfx.cameraToUse]->GetPositionFloat3().x,
					9.0f, gfx.cameras[gfx.cameraToUse]->GetPositionFloat3().z );
		}
		else
		{
			// model movement
			if ( keyboard.KeyIsPressed( 'W' ) )
				CameraMove::MoveForward( gfx.cameras[gfx.cameraToUse], gfx.renderables[0], dt );

			if ( keyboard.KeyIsPressed( 'A' ) )
				CameraMove::MoveLeft( gfx.cameras[gfx.cameraToUse], gfx.renderables[0], dt );

			if ( keyboard.KeyIsPressed( 'S' ) )
				CameraMove::MoveBackward( gfx.cameras[gfx.cameraToUse], gfx.renderables[0], dt );

			if ( keyboard.KeyIsPressed( 'D' ) )
				CameraMove::MoveRight( gfx.cameras[gfx.cameraToUse], gfx.renderables[0], dt );
		}

		// prevent cameras moving under map
		for ( auto const& x : gfx.cameras )
			if ( x.second->GetPositionFloat3().y <= 6.0f )
				x.second->SetPosition( x.second->GetPositionFloat3().x, 6.0f, x.second->GetPositionFloat3().z );

		// light object position
		XMVECTOR lightPosition = gfx.cameras[gfx.cameraToUse]->GetPositionVector();
		lightPosition += gfx.cameras[gfx.cameraToUse]->GetForwardVector() / 2;
		if ( ( ( keyboard.KeyIsPressed( 'C' ) || lightParams.lightStuck ) && lightParams.isEquippable ) || lightParams.lightIntersection )
		{
			lightParams.lightStuck = true;
			lightParams.lightHover = false;
			lightParams.lightIntersection = false;
			lightPosition += gfx.cameras[gfx.cameraToUse]->GetRightVector() / 2;
			lightPosition -= { 0.0f, 0.25f, 0.0f, 0.0f };
			gfx.light.SetPosition( lightPosition );
			gfx.light.SetRotation( gfx.cameras[gfx.cameraToUse]->GetRotationVector() );
		}

		if ( keyboard.KeyIsPressed( 'X' ) && lightParams.lightStuck )
		{
			lightParams.lightStuck = false;
			gfx.light.SetPosition( lightPosition );
		}

		// manage viewports
		if ( keyboard.KeyIsPressed( VK_UP ) )
		{
			viewportParams = { 0 };
			viewportParams.useFull = true;
		}

		if ( keyboard.KeyIsPressed( VK_DOWN ) )
		{
			viewportParams = { 0 };
			viewportParams.useSplit = true;
		}

		if ( keyboard.KeyIsPressed( VK_LEFT ) )
			gfx.controlLeftSide = true;

		if ( keyboard.KeyIsPressed( VK_RIGHT ) )
			gfx.controlLeftSide = false;
	}

	gfx.Update( dt );
}

void Application::Render()
{
	if ( !viewportParams.useSplit )
	{
		gfx.BeginFrame();
		gfx.RenderFrame();
		gfx.EndFrame();
	}
	else
	{
		viewportParams.useLeft = true;
		gfx.BeginFrame();
		gfx.RenderFrame();

		viewportParams.useRight = true;
		gfx.BeginFrame();
		gfx.RenderFrame();

		viewportParams.useFull = true;
		gfx.BeginFrame();
		gfx.EndFrame();
	}
}