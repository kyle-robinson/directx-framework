#include "ImGuiManager.h"
#include "Viewport.h"
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

void ImGuiManager::RenderMainWindow( Graphics& gfx )
{
	if ( ImGui::Begin( "Main Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 0.8f, 1.0f, 1.0f } );
        if ( ImGui::TreeNode( "Spawn Windows" ) )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
            ImGui::Checkbox( "Scene", &spawnWindow.sceneWindow );
            ImGui::Checkbox( "Lighting", &spawnWindow.lightWindow );
            ImGui::Checkbox( "Fog", &spawnWindow.fogWindow );
            ImGui::Checkbox( "Models", &spawnWindow.modelWindow );
            ImGui::Checkbox( "Cameras", &spawnWindow.cameraWindow );
            ImGui::Checkbox( "Stencils", &spawnWindow.stencilWindow );
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

void ImGuiManager::RenderSceneWindow( Graphics& gfx )
{
    if ( ImGui::Begin( "Scene Parameters", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        ImGui::ColorEdit3( "Clear Color", sceneParams.clearColor );
        ImGui::SliderFloat( "Blend Factor", &sceneParams.alphaFactor, 0.0f, 1.0f );
			    
        static int fillGroup = 0;
	    if ( ImGui::RadioButton( "Solid", &fillGroup, 0 ) )
            sceneParams.rasterizerSolid = true;
	    ImGui::SameLine();
	    if ( ImGui::RadioButton( "Wireframe", &fillGroup, 1 ) )
            sceneParams.rasterizerSolid = false;

        ImGui::Checkbox( "Enable Textures", &sceneParams.useTexture );
        ImGui::Checkbox( "Nanosuit Billboarding", &sceneParams.useBillboarding );

        static int activeSampler = 0;
        static bool selectedSampler[3];
        static std::string previewValueSampler = "Anisotropic";
        static const char* samplerList[]{ "Anisotropic", "Bilinear", "Point Sampling" };
        if ( ImGui::BeginCombo( "Sampler Type", previewValueSampler.c_str() ) )
        {
            for ( unsigned int i = 0; i < IM_ARRAYSIZE( samplerList ); i++ )
            {
                const bool isSelected = i == activeSampler;
                if ( ImGui::Selectable( samplerList[i], isSelected ) )
                {
                    activeSampler = i;
                    previewValueSampler = samplerList[i];
                }
            }

            switch ( activeSampler )
            {
                case 0:
                    samplerParams = { 0 };
                    samplerParams.useAnisotropic = true;
                    break;
                case 1:
                    samplerParams = { 0 };
                    samplerParams.useBilinear = true;
                    break;
                case 2:
                    samplerParams = { 0 };
                    samplerParams.usePoint = true;
                    break;
            }

            ImGui::EndCombo();
        }

        static int activeViewport = 0;
        static bool selectedViewport[3];
        static std::string previewValueViewport = "Fullscreen";
        static const char* viewportList[]{ "Fullscreen", "Multi-View", "Split-Screen" };
        if ( ImGui::BeginCombo( "Viewport Type", previewValueViewport.c_str() ) )
        {
            for ( unsigned int i = 0; i < IM_ARRAYSIZE( viewportList ); i++ )
            {
                const bool isSelected = i == activeViewport;
                if ( ImGui::Selectable( viewportList[i], isSelected ) )
                {
                    activeViewport = i;
                    previewValueViewport = viewportList[i];
                }
            }

            switch ( activeViewport )
            {
                case 0:
                    viewportParams = { 0 };
                    sceneParams.multiView = false;
                    viewportParams.useFull = true;
                    break;
                case 1:
                    viewportParams = { 0 };
                    sceneParams.multiView = true;
                    break;
                case 2:
                    viewportParams = { 0 };
                    sceneParams.multiView = false;
                    viewportParams.useSplit = true;
                    break;
            }

            ImGui::EndCombo();
        }
    } ImGui::End();
}

void ImGuiManager::RenderLightWindow( Light& light, ConstantBuffer<CB_PS_light>& cb_ps_light )
{
	if ( ImGui::Begin( "Light Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
        static int lightGroup = 0;
        if ( ImGui::RadioButton( "Directional", &lightGroup, 0 ) )
            cb_ps_light.data.usePointLight = false;
        ImGui::SameLine();
        if ( ImGui::RadioButton( "Point", &lightGroup, 1 ) )
            cb_ps_light.data.usePointLight = true;
        
        if ( cb_ps_light.data.usePointLight )
        {
            if ( lightParams.isEquippable )
                ImGui::Checkbox( "Equip Light", &lightParams.lightStuck );
            if ( !lightParams.isEquippable && !lightParams.lightStuck )
                ImGui::TextColored( { 1.0f, 0.2f, 0.2f, 1.0f }, "Not within range to equip light!" );
        }

        ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 1.0f, 0.8f, 1.0f } );
		if ( ImGui::TreeNode( "Ambient Components" ) )
		{
            ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
			ImGui::ColorEdit3( "Ambient", &cb_ps_light.data.ambientLightColor.x );
			ImGui::SliderFloat( "Intensity", &cb_ps_light.data.ambientLightStrength, 0.0f, 1.0f );
            ImGui::PopStyleColor();
		    ImGui::TreePop();
        }
        if ( cb_ps_light.data.usePointLight )
        {
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

            if ( ImGui::TreeNode( "Light Flicker" ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                ImGui::Checkbox( "Flickering Enabled", &lightParams.lightFlicker );
                ImGui::SliderFloat( "Intensity", &lightParams.flickerAmount, 1.0f, 5.0f );
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
        }
        else
        {
            if ( ImGui::TreeNode( "Directional Components" ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                ImGui::ColorEdit3( "Colour", &cb_ps_light.data.directionalLightColor.x );
                ImGui::DragFloat3( "Position", &cb_ps_light.data.directionalLightPosition.x );
                ImGui::SliderFloat( "Intensity", &cb_ps_light.data.directionalLightIntensity, 0.0f, 10.0f );
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
        }
        light.SetConstantBuffer( cb_ps_light );
        light.UpdateConstantBuffer( cb_ps_light );
        ImGui::PopStyleColor();
	} ImGui::End();
}

void ImGuiManager::RenderFogWindow( ConstantBuffer<CB_VS_fog>& cb_vs_fog )
{
    if ( ImGui::Begin( "Fog Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        ImGui::Checkbox( "Enable Fog", &cb_vs_fog.data.fogEnable );
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
    if ( ImGui::Begin( "Models", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        for ( unsigned int i = 0; i < models.size(); i++ )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, { 0.8f, 1.0f, 0.5f, 1.0f } );
            if ( ImGui::TreeNode( models[i].GetModelName().c_str() ) )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );
                if ( ImGui::TreeNode( "Position" ) )
                {
                    static DirectX::XMFLOAT3 positions = { models[i].GetPositionFloat3() };
                    ImGui::SliderFloat( "X", &positions.x, -40.0f, 40.0f, "%.1f" );
                    ImGui::SliderFloat( "Y", &positions.y, -40.0f, 40.0f, "%.1f" );
                    ImGui::SliderFloat( "Z", &positions.z, -40.0f, 40.0f, "%.1f" );

                    positions.x = XMConvertToRadians( positions.x );
                    positions.y = XMConvertToRadians( positions.y );
                    positions.z = XMConvertToRadians( positions.z );

                    models[i].AdjustPosition( positions );

                    ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
                    if ( ImGui::Button( "Reset Position" ) )
                        models[i].ResetPosition();
                    ImGui::PopStyleColor();

                    ImGui::TreePop();
                }

                if ( ImGui::TreeNode( "Orientation" ) )
                {
                    static DirectX::XMFLOAT3 rotations = { models[i].GetRotationFloat3() };
                    ImGui::SliderFloat( "Pitch", &rotations.x, -2.0f, 2.0f, "%.1f" );
                    ImGui::SliderFloat( "Yaw", &rotations.y, -2.0f, 2.0f, "%.1f" );
                    ImGui::SliderFloat( "Roll", &rotations.z, -2.0f, 2.0f, "%.1f" );
                
                    rotations.x = XMConvertToRadians( rotations.x );
                    rotations.y = XMConvertToRadians( rotations.y );
                    rotations.z = XMConvertToRadians( rotations.z );
                
                    models[i].AdjustRotation( rotations );

                    ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
                    if ( ImGui::Button( "Reset Rotation" ) )
                        models[i].ResetRotation();
                    ImGui::PopStyleColor();

                    ImGui::TreePop();
                }

                if ( ImGui::TreeNode( "Scaling" ) )
                {
                    static DirectX::XMFLOAT3 scales = { models[i].GetScaleFloat3() };
                    ImGui::SliderFloat( "X", &scales.x, -2.0f, 2.0f, "%.001f" );
                    ImGui::SliderFloat( "Y", &scales.y, -2.0f, 2.0f, "%.001f" );
                    ImGui::SliderFloat( "Z", &scales.z, -2.0f, 2.0f, "%.001f" );

                    scales.x = XMConvertToRadians( scales.x );
                    scales.y = XMConvertToRadians( scales.y );
                    scales.z = XMConvertToRadians( scales.z );

                    models[i].AdjustScale( scales.x, scales.y, scales.z );

                    ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
                    if ( ImGui::Button( "Reset Scale" ) )
                        models[i].ResetScale();
                    ImGui::PopStyleColor();

                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
    } ImGui::End();
}

void ImGuiManager::RenderCameraWindow( Graphics& gfx, Camera3D& camera3D, std::string& cameraToUse )
{
    if ( ImGui::Begin( "Camera", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {   
        static int active = 0;
        static bool selectedCamera[3];
        static std::string previewValue = cameraToUse;
        static const char* cameraList[]{ "Main", "Point", "Third" };
        if ( ImGui::BeginCombo( "Active Camera", previewValue.c_str() ) )
        {
            for ( unsigned int i = 0; i < IM_ARRAYSIZE( cameraList ); i++ )
            {
                const bool isSelected = i == active;
                if ( ImGui::Selectable( cameraList[i], isSelected ) )
                {
                    active = i;
                    previewValue = cameraList[i];
                }
            }

            switch ( active )
            {
                case 0: cameraToUse = "Main";  break;
                case 1: cameraToUse = "Point"; break;
                case 2: cameraToUse = "Third"; break;
            }

            ImGui::EndCombo();
        }

        if ( previewValue == "Main" )
        {
            static int cameraGroup = 0;
            if ( ImGui::RadioButton( "Static", &cameraGroup, 0 ) )
                gfx.flyCamera = false;
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Fly", &cameraGroup, 1 ) )
                gfx.flyCamera = true;
        }
        
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

            static float windowWidthF = static_cast<float>( gfx.GetWidth() );
            static float windowHeightF = static_cast<float>( gfx.GetHeight() );
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

void ImGuiManager::RenderStencilWindow( Graphics& gfx )
{
    if ( ImGui::Begin( "Stencil", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        ImGui::Checkbox( "Enable Stencil Mask", &sceneParams.useMask );
        if ( sceneParams.useMask )
        {
            static int maskTypeGroup = 0;
            if ( ImGui::RadioButton( "Circle", &maskTypeGroup, 0 ) )
                sceneParams.circleMask = true;
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Square", &maskTypeGroup, 1 ) )
                sceneParams.circleMask = false;

            if ( sceneParams.circleMask )
            {
                static float width = 200.0f;
                static float height = 200.0f;
                ImGui::SliderFloat( "Width", &width, 100.0f, gfx.GetWidth(), "%10.f" );
                ImGui::SliderFloat( "Height", &height, 100.0f, gfx.GetHeight(), "%10.f" );
                gfx.circle.SetScale( width, height, 1.0f );
                gfx.circle.SetPosition( gfx.GetWidth() / 2 - gfx.circle.GetWidth() / 2,
                    gfx.GetHeight() / 2 - gfx.circle.GetHeight() / 2, 0 );
            }
            else if ( !sceneParams.circleMask )
            {
                static float width = 200.0f;
                static float height = 200.0f;
                ImGui::SliderFloat( "Width", &width, 100.0f, gfx.GetWidth(), "%10.f" );
                ImGui::SliderFloat( "Height", &height, 100.0f, gfx.GetHeight(), "%10.f" );
                gfx.square.SetScale( width, height, 1.0f );
                gfx.square.SetPosition( gfx.GetWidth() / 2 - gfx.square.GetWidth() / 2,
                    gfx.GetHeight() / 2 - gfx.square.GetHeight() / 2, 0 );
            }
        }

        ImGui::Separator();

        ImGui::ColorEdit3( "Light Stencil", &outlineParams.outlineColor.x );
        ImGui::SliderFloat( "Scale", &outlineParams.outlineSize, 1.0f, 2.0f, "%.1f" );
    } ImGui::End();
}