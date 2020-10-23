#pragma once
#ifndef CONSTANTBUFFERTYPES_H
#define CONSTANTBUFFERTYPES_H

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

struct CB_VS_vertexshader_normal
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};

struct CB_PS_pixelshader_normal
{
	DirectX::XMFLOAT4 ambientLight = { 0.2f, 0.2f, 0.2f, 1.0f };
	DirectX::XMFLOAT4 diffuseMaterial = { 0.8f, 0.5f, 0.5f, 1.0f };
	DirectX::XMFLOAT4 diffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 specularLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	float specularPower = 10.0f;
	DirectX::XMFLOAT3 lightDirection = { 0.25f, 0.5f, -1.0f };
	DirectX::XMFLOAT3 eyePos;
};

#endif