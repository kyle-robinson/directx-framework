#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	float gTime;
};

struct CB_VS_vertexshader_water
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	float gTime;
	float waterSpeed;
	float waterAmount;
	float waterHeight;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};