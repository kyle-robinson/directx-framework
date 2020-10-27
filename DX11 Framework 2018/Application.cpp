#include "Application.h"
#include "imgui/imgui.h"

bool Application::Initialize(
	HINSTANCE hInstance,
	const std::string& windowTitle,
	const std::string& windowClass,
	int width,
	int height )
{
	this->timer.Start();

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
    float dt = this->timer.GetMilliSecondsElapsed();
	this->timer.Restart();

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

	float cameraSpeed = 0.002f;

	if ( keyboard.KeyIsPressed( VK_SHIFT ) )
		cameraSpeed = 0.01f;

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

	if ( keyboard.KeyIsPressed( 'E' ) )
		this->gfx.camera.AdjustPosition( XMFLOAT3( 0.0f, -cameraSpeed * dt, 0.0f ) );

	if ( keyboard.KeyIsPressed( 'C' ) )
	{
		XMVECTOR lightPosition = gfx.camera.GetPositionVector();
		lightPosition += this->gfx.camera.GetForwardVector();
		this->gfx.light.SetPosition( lightPosition );
		this->gfx.light.SetRotation( this->gfx.camera.GetRotationFloat3() );
	}

	// constant buffer
	// timers
	/*static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount64();
	if ( dwTimeStart == 0 ) dwTimeStart = dwTimeCur;
	gfx.gTime = ( dwTimeCur - dwTimeStart ) / 1000.0f;
	gfx.cb_vs_vertexshader.data.gTime = gfx.gTime;
	gfx.cb_vs_vertexshader_water.data.gTime = gfx.gTime;

	// matrices
	gfx.cb_vs_vertexshader.data.mView = gfx.camera.GetViewMatrix();
	gfx.cb_vs_vertexshader_water.data.mView = gfx.camera.GetViewMatrix();
	gfx.cb_vs_vertexshader_normal.data.mView = gfx.camera.GetViewMatrix();
	gfx.cb_vs_vertexshader.data.mProjection = gfx.camera.GetProjectionMatrix();
	gfx.cb_vs_vertexshader_water.data.mProjection = gfx.camera.GetProjectionMatrix();
	gfx.cb_vs_vertexshader_normal.data.mProjection = gfx.camera.GetProjectionMatrix();

	// variables
	gfx.cb_vs_vertexshader_water.data.waterSpeed = this->waterSpeed;
	gfx.cb_vs_vertexshader_water.data.waterAmount = this->waterAmount;
	gfx.cb_vs_vertexshader_water.data.waterHeight = this->waterHeight;

	// lighting
	gfx.cb_ps_pixelshader_normal.data.ambientLight = this->ambientLight;
	gfx.cb_ps_pixelshader_normal.data.diffuseLight = this->diffuseLight;
	gfx.cb_ps_pixelshader_normal.data.specularLight = this->specularLight;
	gfx.cb_ps_pixelshader_normal.data.diffuseMaterial = this->diffuseMaterial;
	gfx.cb_ps_pixelshader_normal.data.lightDirection = this->lightDirection;
	gfx.cb_ps_pixelshader_normal.data.specularPower = this->specularPower;
	gfx.cb_ps_pixelshader_normal.data.eyePos = gfx.camera.GetPositionFloat3();*/

    gfx.Update( dt );
}

void Application::Render()
{
	//static float clearColor[] = { 0.0f, 0.5f, 0.5f, 1.0f };

	this->gfx.BeginFrame();
	this->gfx.RenderFrame();

	// render imgui windows
	/*if ( ImGui::Begin( "Main Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		if ( ImGui::CollapsingHeader( "Float Sliders" ) )
		{
			ImGui::ColorEdit3( "Clear Color", clearColor );
			static int fillGroup = 0;
			if ( ImGui::RadioButton( "Solid", &fillGroup, 0 ) )
				gfx.rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			ImGui::SameLine();
			if ( ImGui::RadioButton( "Wireframe", &fillGroup, 1 ) )
				gfx.rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
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
	ImGui::End();*/

	/*if( ImGui::Begin( "Main Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		if ( ImGui::CollapsingHeader( "Scene Parameters" ) )
		{
			ImGui::ColorEdit3( "Clear Colour", clearColor );
			ImGui::DragFloat( "Rotation Multiplier", &gfx.multiplier, 0.01f, -1.0f, 1.0f );
			static int fillGroup = 0;
			if ( ImGui::RadioButton( "Solid", &fillGroup, 0 ) )
				gfx.rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			ImGui::SameLine();
			if ( ImGui::RadioButton( "Wireframe", &fillGroup, 1 ) )
				gfx.rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		}
		if ( ImGui::CollapsingHeader( "Water Parameters" ) )
		{
			ImGui::DragFloat( "Speed", &this->waterSpeed, 0.01f, 0.0f, 1.0f );
			ImGui::DragFloat( "Amount", &this->waterAmount, 0.0001f, 0.0f, 0.02f );
			ImGui::DragFloat( "Height", &this->waterHeight, 0.01f, 0.0f, 1.0f );
		}
		if ( ImGui::CollapsingHeader( "Lighting Parameters" ) )
		{
			ImGui::ColorEdit4( "Ambient", &this->ambientLight.x );
			ImGui::ColorEdit4( "Diffuse", &this->diffuseLight.x );
			ImGui::ColorEdit4( "Specular", &this->specularLight.x );
			ImGui::ColorEdit4( "Material", &this->diffuseMaterial.x );
			ImGui::SliderFloat3( "Light Direction", &this->lightDirection.x, -1.0f, 1.0f );
			ImGui::SliderFloat( "Specular Power", &this->specularPower, 0.0f, 20.0f );
		}
		if ( ImGui::CollapsingHeader( "Application Info" ) )
		{
			ImGui::Text( "Processor Type: %u", this->siSysInfo.dwProcessorType );
			ImGui::Text( "Processor Count: %u", this->siSysInfo.dwNumberOfProcessors );
			ImGui::Text( "OEM ID: %u", this->siSysInfo.dwOemId );
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
	ImGui::End();*/

	this->gfx.EndFrame();
}