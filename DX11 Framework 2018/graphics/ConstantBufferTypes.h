#pragma once
#ifndef CONSTANTBUFFERTYPES_H
#define CONSTANTBUFFERTYPES_H

#include <DirectXMath.h>

struct CB_VS_vertexShader
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
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
	DirectX::XMFLOAT4 ambientLight;
	DirectX::XMFLOAT4 diffuseMaterial;
	DirectX::XMFLOAT4 diffuseLight;
	DirectX::XMFLOAT4 specularLight;
	DirectX::XMFLOAT3 lightDirection;
	float specularPower;
	DirectX::XMFLOAT3 eyePos;
};

struct CB_PS_light
{
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength;
	DirectX::XMFLOAT3 specularLightColor;
	float specularLightIntensity;
	float specularLightPower;
	DirectX::XMFLOAT3 dynamicLightPosition;
	float lightConstant;
	float lightLinear;
	float lightQuadratic;
};

#endif