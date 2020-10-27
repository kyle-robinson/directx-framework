#pragma once
#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include "Light.h"
#include <d3d11.h>

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();
	void RenderMainWindow( ID3D11DeviceContext* context, float clearColor[4],
		ID3D11RasterizerState* rasterizerState_Solid,
		ID3D11RasterizerState* rasterizerState_Wireframe );
	void RenderLightWindow( Light& light, ConstantBuffer<CB_PS_light>& cb_ps_light );
private:
	SYSTEM_INFO siSysInfo;
};

#endif