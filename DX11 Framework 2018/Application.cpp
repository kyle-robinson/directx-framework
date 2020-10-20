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
	GetSystemInfo( &siSysInfo );

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
    float dt = timer.GetMiliSecondsElapsed();
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
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetForwardVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( 'A' ) )
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetLeftVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( 'S' ) )
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetBackwardVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( 'D' ) )
		this->gfx.camera.AdjustPosition( this->gfx.camera.GetRightVector() * cameraSpeed * dt );

	if ( keyboard.KeyIsPressed( VK_SPACE ) )
		this->gfx.camera.AdjustPosition( XMFLOAT3( 0.0f, cameraSpeed * dt, 0.0f ) );

	if ( keyboard.KeyIsPressed( VK_SHIFT ) )
		this->gfx.camera.AdjustPosition( XMFLOAT3( 0.0f, -cameraSpeed * dt, 0.0f ) );

	gfx.multiplier = multiplier;
	gfx.waterSpeed = waterSpeed;
	gfx.waterAmount = waterAmount;
	gfx.waterHeight = waterHeight;
    gfx.Update();
}

void Application::Render()
{
	static float clearColor[] = { 0.0f, 0.5f, 0.5f, 1.0f };

	this->gfx.BeginFrame( clearColor );
	this->gfx.RenderFrame();

	if( ImGui::Begin( "Main Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		if ( ImGui::CollapsingHeader( "Scene Parameters" ) )
		{
			ImGui::ColorEdit3( "Clear Colour", clearColor );
			ImGui::DragFloat( "Rotation Multiplier", &multiplier, 0.01f, -1.0f, 1.0f );
			static int fillGroup = 0;
			if ( ImGui::RadioButton( "Solid", &fillGroup, 0 ) )
				gfx.rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			ImGui::SameLine();
			if ( ImGui::RadioButton( "Wireframe", &fillGroup, 1 ) )
				gfx.rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		}
		if ( ImGui::CollapsingHeader( "Water Parameters" ) )
		{
			ImGui::DragFloat( "Speed", &waterSpeed, 0.01f, 0.0f, 1.0f );
			ImGui::DragFloat( "Amount", &waterAmount, 0.0001f, 0.0f, 0.02f );
			ImGui::DragFloat( "Height", &waterHeight, 0.01f, 0.0f, 1.0f );
		}
		if ( ImGui::CollapsingHeader( "Application Info" ) )
		{
			ImGui::Text( "Processor Type: %u", siSysInfo.dwProcessorType );
			ImGui::Text( "Processor Count: %u", siSysInfo.dwNumberOfProcessors );
			ImGui::Text( "OEM ID: %u", siSysInfo.dwOemId );
			ImGui::NewLine();
			ImGui::Text( "Frametime: %.3f / Framerate: (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
		}
		if ( ImGui::CollapsingHeader( "About" ) )
		{
			ImGui::Text( "DirectX Framework by Kyle Robinson" );
			ImGui::NewLine();
			ImGui::Text( "LinkedIn: linkedin.com/in/kylerobinsongames/" );
			ImGui::Text( "Email: kylerobinson456@outlook.com" );
			ImGui::Text( "Twitter: @KyleRobinson42" );
		}
	}
	ImGui::End();

	this->gfx.EndFrame();
}