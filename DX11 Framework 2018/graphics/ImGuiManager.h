#pragma once
#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include "Light.h"
#include "Camera3D.h"
#include <d3d11.h>

struct RenderableGameObject;
struct Drawable;

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();
	void Initialize( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context ) const noexcept;
	void BeginRender() const noexcept;
	void EndRender() const noexcept;
	void RenderMainWindow( ID3D11DeviceContext* context, float& alphaFactor, bool& useTexture, float clearColor[4],
		bool& rasterizerSolid, bool& samplerAnisotropic, bool& multiView, bool& useMask, bool& circleMask );
	void RenderLightWindow( Light& light, ConstantBuffer<CB_PS_light>& cb_ps_light );
	void RenderFogWindow( ConstantBuffer<CB_VS_fog>& cb_vs_fog );
	void RenderModelWindow( std::vector<RenderableGameObject>& models );
	void RenderCameraWindow( Camera3D& camera3D, UINT windowWidth, UINT windowHeight );
private:
	SYSTEM_INFO siSysInfo;
};

#endif