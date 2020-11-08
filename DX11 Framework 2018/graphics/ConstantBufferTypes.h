#pragma once
#ifndef CONSTANTBUFFERTYPES_H
#define CONSTANTBUFFERTYPES_H

#include <DirectXMath.h>

struct CB_VS_matrix
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};

struct CB_VS_matrix_2D
{
	DirectX::XMMATRIX wvpMatrix;
};

struct CB_VS_fullscreen
{
	bool multiView;
};

struct CB_VS_fog
{
	DirectX::XMFLOAT3 fogColor;
	float fogStart;
	float fogEnd;
	bool fogEnable;
};

struct CB_VS_water
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	float gTime;
	float waterSpeed;
	float waterAmount;
	float waterHeight;
};

struct CB_PS_alpha
{
	float alpha = 1.0f;
};

struct CB_PS_light
{
	alignas( 16 ) DirectX::XMFLOAT3 ambientLightColor;
	alignas( 16 ) DirectX::XMFLOAT3 dynamicLightColor;
	alignas( 16 ) DirectX::XMFLOAT3 specularLightColor;
	alignas( 16 ) DirectX::XMFLOAT3 dynamicLightPosition;
	float ambientLightStrength;
	float dynamicLightStrength;
	float specularLightIntensity;
	float specularLightPower;
	float lightConstant;
	float lightLinear;
	float lightQuadratic;
	float alphaFactor;
	bool useTexture;
};

struct CB_PS_lightDirect
{
	alignas( 16 ) DirectX::XMFLOAT3 dynamicLightColor;
	float alphaFactor;
	bool useTexture;
};

#endif