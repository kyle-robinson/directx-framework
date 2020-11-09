#include "ImGuiManager.h"
#include "ModelData.h"
#include "RenderableGameObject.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

ImGuiManager::ImGuiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
    GetSystemInfo( &siSysInfo );
}

ImGuiManager::~ImGuiManager()
{
	ImGui::DestroyContext();
}

void ImGuiManager::Initialize( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context ) const noexcept
{
    ImGui_ImplWin32_Init( hWnd );
    ImGui_ImplDX11_Init( device, context );
}

void ImGuiManager::BeginRender() const noexcept
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndRender() const noexcept
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
}

void ImGuiManager::RenderMainWindow( ID3D11DeviceContext* context, float& alphaFactor, bool& useTexture, float clearColor[4],
    bool& rasterizerSolid, bool& samplerAnisotropic, bool& multiView, bool& useMask, bool& circleMask )
{
	if ( ImGui::Begin( "Main Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 0.8f, 1.0f, 1.0f } );

        if ( ImGui::TreeNode( "Scene Parameters" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            {
                ImGui::ColorEdit3( "Clear Color", clearColor );
                ImGui::SliderFloat( "Blend Factor", &alphaFactor, 0.0f, 1.0f );
			    
                static int fillGroup = 0;
			    if ( ImGui::RadioButton( "Solid", &fillGroup, 0 ) )
				    rasterizerSolid = true;
			    ImGui::SameLine();
			    if ( ImGui::RadioButton( "Wireframe", &fillGroup, 1 ) )
                    rasterizerSolid = false;

                static int textureGroup = 0;
                if ( ImGui::RadioButton( "Enable Textures", &textureGroup, 0 ) )
                    useTexture = true;
                ImGui::SameLine();
                if ( ImGui::RadioButton( "Disable Textures", &textureGroup, 1 ) )
                    useTexture = false;

                static int filterGroup = 0;
                if ( ImGui::RadioButton( "Anisotropic", &filterGroup, 0 ) )
                    samplerAnisotropic = true;
                ImGui::SameLine();
                if ( ImGui::RadioButton( "Point", &filterGroup, 1 ) )
                    samplerAnisotropic = false;

                static int viewGroup = 0;
                if ( ImGui::RadioButton( "Normal", &viewGroup, 0 ) )
                    multiView = false;
                ImGui::SameLine();
                if ( ImGui::RadioButton( "Multi-View", &viewGroup, 1 ) )
                    multiView = true;

                static int maskGroup = 0;
                if ( ImGui::RadioButton( "Disable Mask", &maskGroup, 0 ) )
                    useMask = false;
                ImGui::SameLine();
                if ( ImGui::RadioButton( "Enable Mask", &maskGroup, 1 ) )
                    useMask = true;

                if ( useMask )
                {
                    static int maskTypeGroup = 0;
                    if ( ImGui::RadioButton( "Circle", &maskTypeGroup, 0 ) )
                        circleMask = true;
                    ImGui::SameLine();
                    if ( ImGui::RadioButton( "Square", &maskTypeGroup, 1 ) )
                        circleMask = false;
                }
            }
            ImGui::PopStyleColor();
            ImGui::TreePop();
        }

        if ( ImGui::TreeNode( "Scene Controls" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 0.8f, 0.5f, 1.0f } );

            if ( ImGui::TreeNode( "Camera Controls" ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                {
                    ImGui::Text( "W\t\t->\tForward" );
                    ImGui::Text( "A\t\t->\tLeft" );
                    ImGui::Text( "S\t\t->\tBackward" );
                    ImGui::Text( "D\t\t->\tRight" );
                    ImGui::Text( "E\t\t->\tDownward" );
                    ImGui::Text( "SPACE\t->\tUpward" );
                    ImGui::Text( "SHIFT\t->\tMove Faster" );
                    ImGui::Text( "RMB  \t->\tRotate Camera" );
                }
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
            
            if ( ImGui::TreeNode( "Light Controls" ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                {
                    ImGui::Text( "C\t\t->\tMove Light to Camera" );
                }
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
            ImGui::TreePop();
        }

        if ( ImGui::TreeNode( "Application Info" ) )
		{
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            {
			    ImGui::Text( "Processor Type: %u", siSysInfo.dwProcessorType );
			    ImGui::Text( "Processor Count: %u", siSysInfo.dwNumberOfProcessors );
			    ImGui::Text( "OEM ID: %u", siSysInfo.dwOemId );
			    ImGui::NewLine();
			    ImGui::Text( "Frametime: %.3f / Framerate: (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
            }
            ImGui::PopStyleColor();
		    ImGui::TreePop();
        }

		if ( ImGui::TreeNode( "Social Links" ) )
		{
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            {
			    ImGui::Text( "DirectX Framework by Kyle Robinson" );
			    ImGui::NewLine();
			    ImGui::Text( "LinkedIn: linkedin.com/in/kylerobinsongames/" );
			    ImGui::Text( "Email: kylerobinson456@outlook.com" );
			    ImGui::Text( "Twitter: @KyleRobinson42" );
            }
            ImGui::PopStyleColor();
		    ImGui::TreePop();
        }

        ImGui::PopStyleColor();
    } ImGui::End();
}

void ImGuiManager::RenderLightWindow( Light& light, ConstantBuffer<CB_PS_light>& cb_ps_light )
{
	if ( ImGui::Begin( "Light Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
        static int lightGroup = 0;
        if ( ImGui::RadioButton( "Directional", &lightGroup, 0 ) )
            cb_ps_light.data.usePointLight = false;
        ImGui::SameLine();
        if ( ImGui::RadioButton( "Point", &lightGroup, 1 ) )
            cb_ps_light.data.usePointLight = true;

        ImGui::SliderFloat( "Ground Intensity", &cb_ps_light.data.quadIntensity, 0.0f, 1.0f );
        
        ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 1.0f, 0.8f, 1.0f } );

        if ( cb_ps_light.data.usePointLight )
        {
		    if ( ImGui::TreeNode( "Ambient Components" ) )
		    {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                {
			        ImGui::ColorEdit3( "Ambient", &cb_ps_light.data.ambientLightColor.x );
			        ImGui::SliderFloat( "Intensity", &cb_ps_light.data.ambientLightStrength, 0.0f, 1.0f );
                }
                ImGui::PopStyleColor();
		        ImGui::TreePop();
            }

		    if ( ImGui::TreeNode( "Diffuse Components" ) )
		    {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                {
                    ImGui::ColorEdit3( "Diffuse", &cb_ps_light.data.dynamicLightColor.x );
			        ImGui::SliderFloat( "Intensity", &cb_ps_light.data.dynamicLightStrength, 0.0f, 10.0f );
                }
                ImGui::PopStyleColor();
		        ImGui::TreePop();
            }

		    if ( ImGui::TreeNode( "Specular Components" ) )
		    {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                {
			        ImGui::ColorEdit3( "Specular", &cb_ps_light.data.specularLightColor.x );
			        ImGui::SliderFloat( "Intensity", &cb_ps_light.data.specularLightIntensity, 0.0f, 20.0f );
			        ImGui::SliderFloat( "Glossiness", &cb_ps_light.data.specularLightPower, 0.0f, 100.0f );
                }
                ImGui::PopStyleColor();
		        ImGui::TreePop();
            }

		    if ( ImGui::TreeNode( "Attenuation" ) )
		    {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                {
			        ImGui::SliderFloat( "Constant", &cb_ps_light.data.lightConstant, 0.05f, 10.0f, "%.2f", 4 );
			        ImGui::SliderFloat( "Linear", &cb_ps_light.data.lightLinear, 0.0001f, 4.0f, "%.4f", 8 );
			        ImGui::SliderFloat( "Quadratic", &cb_ps_light.data.lightQuadratic, 0.0000001f, 1.0f, "%.7f", 10 );
                }
                ImGui::PopStyleColor();
		        ImGui::TreePop();
            }
        }
        else
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            ImGui::ColorEdit3( "Colour", &cb_ps_light.data.directionalLightColor.x );
            ImGui::DragFloat3( "Position", &cb_ps_light.data.directionalLightPosition.x );
            ImGui::SliderFloat( "Intensity", &cb_ps_light.data.directionalLightIntensity, 0.0f, 10.0f );
            ImGui::PopStyleColor();
        }
        light.SetConstantBuffer( cb_ps_light );
        light.UpdateConstantBuffer( cb_ps_light );

        ImGui::PopStyleColor();
	} ImGui::End();
}

void ImGuiManager::RenderFogWindow( ConstantBuffer<CB_VS_fog>& cb_vs_fog )
{
    if ( ImGui::Begin( "Fog Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        static int fogGroup = 0;
        if ( ImGui::RadioButton( "Disable Fog", &fogGroup, 0 ) )
            cb_vs_fog.data.fogEnable = false;
        ImGui::SameLine();
        if ( ImGui::RadioButton( "Enable Fog", &fogGroup, 1 ) )
            cb_vs_fog.data.fogEnable = true;

        if ( cb_vs_fog.data.fogEnable )
        {
            ImGui::ColorEdit3( "Fog Colour", &cb_vs_fog.data.fogColor.x );
            ImGui::SliderFloat( "Start Distance", &cb_vs_fog.data.fogStart, 1.0f, 10.0f );
            ImGui::SliderFloat( "End Distance", &cb_vs_fog.data.fogEnd, 10.0f, 100.0f  );
        }
    } ImGui::End();
}

void ImGuiManager::RenderModelWindow( std::vector<RenderableGameObject>& models )
{    
    if ( ImGui::Begin( "Models", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        for ( unsigned int i = 0; i < models.size(); i++ )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 0.8f, 1.0f, 0.5f, 1.0f } );
            if ( ImGui::TreeNode( models.at( i ).GetModelName().c_str() ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            
                DirectX::XMFLOAT3 positions = { models.at( i ).GetPositionFloat3().x,
                    models.at( i ).GetPositionFloat3().y, models.at( i ).GetPositionFloat3().z };
                ImGui::DragFloat3( "Position", &positions.x, 2.0f, -200.0f, 200.0f );
                models.at( i ).SetPosition( positions );

                DirectX::XMFLOAT3 rotations = { models.at( i ).GetRotationFloat3().x,
                    models.at( i ).GetRotationFloat3().y, models.at( i ).GetRotationFloat3().z };
                ImGui::DragFloat3( "Rotation", &rotations.x, 0.1f, -3.0f, 3.0f );
                models.at( i ).SetRotation( rotations );

                DirectX::XMFLOAT3 scales = { models.at( i ).GetScaleFloat3().x,
                    models.at( i ).GetScaleFloat3().y, models.at( i ).GetScaleFloat3().z };
                ImGui::DragFloat3( "Scale", &scales.x, 0.1f, 0.1f, 2.0f );
                models.at( i ).SetScale( scales.x, scales.y, scales.z );

                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
    } ImGui::End();
}