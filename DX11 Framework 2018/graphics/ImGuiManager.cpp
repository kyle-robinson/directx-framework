#include "ImGuiManager.h"
#include "imgui/imgui.h"

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

void ImGuiManager::RenderMainWindow( ID3D11DeviceContext* context, float clearColor[4], bool& useTexture, float& alphaFactor,
    bool& rasterizerSolid, bool& samplerAnisotropic, bool& multiView )
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
            }
            ImGui::PopStyleColor();
            ImGui::TreePop();
        }

        if ( ImGui::TreeNode( "Scene Controls" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 0.8f, 0.5f, 1.0f } );

            if ( ImGui::TreeNode( "Camera3D Controls" ) )
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
                    ImGui::Text( "RMB  \t->\tRotate Camera3D" );
                }
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
            
            if ( ImGui::TreeNode( "Light Controls" ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                {
                    ImGui::Text( "C\t\t->\tMove Light to Camera3D" );
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
        ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 1.0f, 0.8f, 1.0f } );

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
                ImGui::ColorEdit3( "Diffuse", &light.lightColor.x );
			    ImGui::SliderFloat( "Intensity", &light.lightStrength, 0.0f, 10.0f );
            }
            ImGui::PopStyleColor();
		    ImGui::TreePop();
        }

		if ( ImGui::TreeNode( "Specular Components" ) )
		{
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            {
			    ImGui::ColorEdit3( "Specular", &light.specularColor.x );
			    ImGui::SliderFloat( "Intensity", &light.specularIntensity, 0.0f, 10.0f );
			    ImGui::SliderFloat( "Glossiness", &light.specularPower, 0.0f, 10.0f );
            }
            ImGui::PopStyleColor();
		    ImGui::TreePop();
        }

		if ( ImGui::TreeNode( "Attenuation" ) )
		{
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            {
			    ImGui::SliderFloat( "Constant", &light.constant, 0.05f, 10.0f, "%.2f", 4 );
			    ImGui::SliderFloat( "Linear", &light.linear, 0.0001f, 4.0f, "%.4f", 8 );
			    ImGui::SliderFloat( "Quadratic", &light.quadratic, 0.0000001f, 1.0f, "%.7f", 10 );
            }
            ImGui::PopStyleColor();
		    ImGui::TreePop();
        }

        ImGui::PopStyleColor();
	} ImGui::End();
}