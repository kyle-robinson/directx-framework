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

    // Setup render state
    if ( keyboard.KeyIsPressed( VK_F1 ) )
        gfx.rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    if ( keyboard.KeyIsPressed( VK_F2 ) )
        gfx.rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

    // Animate the cube
    static float multiplier = 1.0f;
    if ( keyboard.KeyIsPressed( VK_UP ) )
        multiplier += 0.01f;
    if ( keyboard.KeyIsPressed( VK_DOWN ) )
        multiplier -= 0.01f;
	gfx.multiplier = multiplier;

    gfx.Update();
}

void Application::Render()
{
	static float clearColor[] = { 0.0f, 0.5f, 0.5f, 1.0f };

	this->gfx.BeginFrame( clearColor );
	this->gfx.RenderFrame();

	if( ImGui::Begin( "Main Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{

	}
	ImGui::End();

	this->gfx.EndFrame();
}