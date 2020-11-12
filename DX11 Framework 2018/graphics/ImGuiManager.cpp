#include "ImGuiManager.h"
#include "ModelData.h"
#include "GraphicsResource.h"
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

void ImGuiManager::RenderMainWindow( Graphics& gfx, ID3D11DeviceContext* context )
{
	if ( ImGui::Begin( "Main Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 0.8f, 1.0f, 1.0f } );
        if ( ImGui::TreeNode( "Scene Parameters" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            ImGui::ColorEdit3( "Clear Color", gfx.sceneParams.clearColor );
            ImGui::SliderFloat( "Blend Factor", &gfx.sceneParams.alphaFactor, 0.0f, 1.0f );
			    
            static int fillGroup = 0;
			if ( ImGui::RadioButton( "Solid", &fillGroup, 0 ) )
                gfx.sceneParams.rasterizerSolid = true;
			ImGui::SameLine();
			if ( ImGui::RadioButton( "Wireframe", &fillGroup, 1 ) )
                gfx.sceneParams.rasterizerSolid = false;

            static int textureGroup = 0;
            if ( ImGui::RadioButton( "Enable Textures", &textureGroup, 0 ) )
                gfx.sceneParams.useTexture = true;
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Disable Textures", &textureGroup, 1 ) )
                gfx.sceneParams.useTexture = false;

            static int filterGroup = 0;
            if ( ImGui::RadioButton( "Anisotropic", &filterGroup, 0 ) )
                gfx.sceneParams.samplerAnisotropic = true;
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Point", &filterGroup, 1 ) )
                gfx.sceneParams.samplerAnisotropic = false;

            static int viewGroup = 0;
            if ( ImGui::RadioButton( "Normal", &viewGroup, 0 ) )
                gfx.sceneParams.multiView = false;
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Multi-View", &viewGroup, 1 ) )
                gfx.sceneParams.multiView = true;

            static int maskGroup = 0;
            if ( ImGui::RadioButton( "Disable Mask", &maskGroup, 0 ) )
                gfx.sceneParams.useMask = false;
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Enable Mask", &maskGroup, 1 ) )
                gfx.sceneParams.useMask = true;

            if ( gfx.sceneParams.useMask )
            {
                static int maskTypeGroup = 0;
                if ( ImGui::RadioButton( "Circle", &maskTypeGroup, 0 ) )
                    gfx.sceneParams.circleMask = true;
                ImGui::SameLine();
                if ( ImGui::RadioButton( "Square", &maskTypeGroup, 1 ) )
                    gfx.sceneParams.circleMask = false;
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
                ImGui::Text( "W\t\t->\tForward" );
                ImGui::Text( "A\t\t->\tLeft" );
                ImGui::Text( "S\t\t->\tBackward" );
                ImGui::Text( "D\t\t->\tRight" );
                ImGui::Text( "E\t\t->\tDownward" );
                ImGui::Text( "SPACE\t->\tUpward" );
                ImGui::Text( "SHIFT\t->\tMove Faster" );
                ImGui::Text( "RMB  \t->\tRotate Camera" );
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
            
            if ( ImGui::TreeNode( "Light Controls" ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                ImGui::Text( "C\t\t->\tMove Light to Camera" );
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
            ImGui::TreePop();
        }

        if ( ImGui::TreeNode( "Application Info" ) )
		{
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
			ImGui::Text( "Processor Type: %u", siSysInfo.dwProcessorType );
			ImGui::Text( "Processor Count: %u", siSysInfo.dwNumberOfProcessors );
			ImGui::Text( "OEM ID: %u", siSysInfo.dwOemId );
			ImGui::NewLine();
			ImGui::Text( "Frametime: %.3f / Framerate: (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
            ImGui::PopStyleColor();
		    ImGui::TreePop();
        }

		if ( ImGui::TreeNode( "Social Links" ) )
		{
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
			ImGui::Text( "DirectX Framework by Kyle Robinson" );
			ImGui::NewLine();
			ImGui::Text( "LinkedIn: linkedin.com/in/kylerobinsongames/" );
			ImGui::Text( "Email: kylerobinson456@outlook.com" );
			ImGui::Text( "Twitter: @KyleRobinson42" );
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
			    ImGui::ColorEdit3( "Ambient", &cb_ps_light.data.ambientLightColor.x );
			    ImGui::SliderFloat( "Intensity", &cb_ps_light.data.ambientLightStrength, 0.0f, 1.0f );
                ImGui::PopStyleColor();
		        ImGui::TreePop();
            }

		    if ( ImGui::TreeNode( "Diffuse Components" ) )
		    {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                ImGui::ColorEdit3( "Diffuse", &cb_ps_light.data.dynamicLightColor.x );
			    ImGui::SliderFloat( "Intensity", &cb_ps_light.data.dynamicLightStrength, 0.0f, 10.0f );
                ImGui::PopStyleColor();
		        ImGui::TreePop();
            }

		    if ( ImGui::TreeNode( "Specular Components" ) )
		    {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
			    ImGui::ColorEdit3( "Specular", &cb_ps_light.data.specularLightColor.x );
			    ImGui::SliderFloat( "Intensity", &cb_ps_light.data.specularLightIntensity, 0.0f, 20.0f );
			    ImGui::SliderFloat( "Glossiness", &cb_ps_light.data.specularLightPower, 0.0f, 100.0f );
                ImGui::PopStyleColor();
		        ImGui::TreePop();
            }

		    if ( ImGui::TreeNode( "Attenuation" ) )
		    {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
			    ImGui::SliderFloat( "Constant", &cb_ps_light.data.lightConstant, 0.05f, 10.0f, "%.2f", 4 );
			    ImGui::SliderFloat( "Linear", &cb_ps_light.data.lightLinear, 0.0001f, 4.0f, "%.4f", 8 );
			    ImGui::SliderFloat( "Quadratic", &cb_ps_light.data.lightQuadratic, 0.0000001f, 1.0f, "%.7f", 10 );
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
            ImGui::SliderFloat( "Start Distance", &cb_vs_fog.data.fogStart, 1.0f, cb_vs_fog.data.fogEnd - 0.1f );
            ImGui::SliderFloat( "End Distance", &cb_vs_fog.data.fogEnd, cb_vs_fog.data.fogStart + 0.1f, 100.0f  );
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
            
                // model positions
                static DirectX::XMFLOAT3 positions = { models.at( i ).GetPositionFloat3() };
                ImGui::SliderFloat3( "Position", &positions.x, -40.0f, 40.0f, "%.1f" );

                positions.x = XMConvertToRadians( positions.x );
                positions.y = XMConvertToRadians( positions.y );
                positions.z = XMConvertToRadians( positions.z );

                models.at( i ).AdjustPosition( positions );

                ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
                if ( ImGui::Button( "Reset Position" ) )
                    models.at( i ).ResetPosition();
                ImGui::PopStyleColor();

                // model rotations
                static DirectX::XMFLOAT3 rotations = { models.at( i ).GetRotationFloat3() };
                ImGui::SliderFloat3( "Rotation", &rotations.x, -2.0f, 2.0f, "%.1f" );
                
                rotations.x = XMConvertToRadians( rotations.x );
                rotations.y = XMConvertToRadians( rotations.y );
                rotations.z = XMConvertToRadians( rotations.z );
                
                models.at( i ).AdjustRotation( rotations );

                ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
                if ( ImGui::Button( "Reset Rotation" ) )
                    models.at( i ).ResetRotation();
                ImGui::PopStyleColor();

                // model scales
                static DirectX::XMFLOAT3 scales = { models.at( i ).GetScaleFloat3() };
                ImGui::SliderFloat3( "Scale", &scales.x, -2.0f, 2.0f, "%.001f" );

                scales.x = XMConvertToRadians( scales.x );
                scales.y = XMConvertToRadians( scales.y );
                scales.z = XMConvertToRadians( scales.z );

                models.at( i ).AdjustScale( scales.x, scales.y, scales.z );

                ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
                if ( ImGui::Button( "Reset Scale" ) )
                    models.at( i ).ResetScale();
                ImGui::PopStyleColor();

                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
    } ImGui::End();
}

void ImGuiManager::RenderCameraWindow( Camera3D& camera3D, UINT windowWidth, UINT windowHeight )
{
    if ( ImGui::Begin( "Camera", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {   
        ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 1.0f, 0.8f, 1.0f } );
        if ( ImGui::TreeNode( "Position" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );

            static DirectX::XMFLOAT3 positions = { camera3D.GetPositionFloat3() };
            ImGui::SliderFloat( "X", &positions.x, -40.0f, 40.0f, "%.1f" );
            ImGui::SliderFloat( "Y", &positions.y, -40.0f, 40.0f, "%.1f" );
            ImGui::SliderFloat( "Z", &positions.z, -40.0f, 40.0f, "%.1f" );

            positions.x = XMConvertToRadians( positions.x );
            positions.y = XMConvertToRadians( positions.y );
            positions.z = XMConvertToRadians( positions.z );

            camera3D.AdjustPosition( positions );

            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
            if ( ImGui::Button( "Reset Position" ) )
                camera3D.ResetPosition();
            ImGui::PopStyleColor();

            ImGui::TreePop();
            ImGui::PopStyleColor();
        }

        if ( ImGui::TreeNode( "Orientation" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );

            static DirectX::XMFLOAT3 rotations = { camera3D.GetRotationFloat3() };
            ImGui::SliderFloat( "Pitch", &rotations.x, -1.0f, 1.0f );
            ImGui::SliderFloat( "Yaw", &rotations.y, -2.0f, 2.0f );
            ImGui::SliderFloat( "Roll", &rotations.z, -1.0f, 1.0f );

            rotations.x = XMConvertToRadians( rotations.x );
            rotations.y = XMConvertToRadians( rotations.y );
            rotations.z = XMConvertToRadians( rotations.z );

            camera3D.AdjustRotation( rotations );

            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
            if ( ImGui::Button( "Reset Orientation" ) )
                camera3D.ResetOrientation();
            ImGui::PopStyleColor();

            ImGui::TreePop();
            ImGui::PopStyleColor();
        }

        if ( ImGui::TreeNode( "Projection" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );

            float fovDegrees = camera3D.GetFoVDegrees();
            float nearZ = camera3D.GetNearZ();
            float farZ = camera3D.GetFarZ();

            ImGui::SliderFloat( "FoV", &fovDegrees, 70.0f, 110.0f, "%1.f", 1.5f );
            ImGui::SliderFloat( "Near Z", &nearZ, 0.01f, farZ - 0.01f, "%.2f", 2.0f );
            ImGui::SliderFloat( "Far Z", &farZ, nearZ + 0.01f, 1000.0f, "%.2f", 4.0f );

            static float windowWidthF = static_cast<float>( windowWidth );
            static float windowHeightF = static_cast<float>( windowHeight );
            static float aspectRatio = windowWidthF / windowHeightF;

            camera3D.SetProjectionValues( fovDegrees, aspectRatio, nearZ, farZ );

            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
            if ( ImGui::Button( "Reset Projection" ) )
                camera3D.ResetProjection( aspectRatio );
            ImGui::PopStyleColor();

            ImGui::TreePop();
            ImGui::PopStyleColor();
        }
        ImGui::PopStyleColor();
    } ImGui::End();
}