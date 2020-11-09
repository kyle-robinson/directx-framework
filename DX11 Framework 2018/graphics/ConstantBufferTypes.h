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
	alignas( 16 ) bool multiView;
};

struct CB_VS_fog
{
	alignas( 16 ) DirectX::XMFLOAT3 fogColor;
	alignas( 16 ) float fogStart;
	alignas( 16 ) float fogEnd;
	alignas( 16 ) bool fogEnable;
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
	bool usePointLight;

	alignas( 16 ) DirectX::XMFLOAT3 directionalLightColor;
	alignas( 16 ) DirectX::XMFLOAT3 directionalLightPosition;
	float directionalLightIntensity;
	bool useQuad;
};

struct CB_PS_scene
{
	float alphaFactor;
	bool useTexture;
};

#endif