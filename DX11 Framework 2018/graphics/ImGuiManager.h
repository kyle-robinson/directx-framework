#pragma once
#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include "Light.h"
#include "Camera3D.h"
#include <d3d11.h>

struct RenderableGameObject;
struct Drawable;
class Graphics;

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();
	void Initialize( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context ) const noexcept;
	void BeginRender() const noexcept;
	void EndRender() const noexcept;
	void RenderMainWindow( Graphics& gfx, ID3D11DeviceContext* context );
	void RenderLightWindow( Light& light, ConstantBuffer<CB_PS_light>& cb_ps_light );
	void RenderFogWindow( ConstantBuffer<CB_VS_fog>& cb_vs_fog );
	void RenderModelWindow( std::vector<RenderableGameObject>& models );
	void RenderCameraWindow( Camera3D& camera3D, UINT windowWidth, UINT windowHeight );
private:
	SYSTEM_INFO siSysInfo;
};

#endif