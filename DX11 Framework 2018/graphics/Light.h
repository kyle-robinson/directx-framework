#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "RenderableGameObject.h"

class Light : public RenderableGameObject
{
public:
	bool Initialize( ID3D11Device* device, ID3D11DeviceContext* context,
		ConstantBuffer<CB_VS_matrix>& cb_vs_vertexshader );
	void SetConstantBuffer( ConstantBuffer<CB_PS_light>& cb_ps_light );
	void UpdateConstantBuffer( ConstantBuffer<CB_PS_light>& cb_ps_light );
	void SetConstantBuffer( ConstantBuffer<CB_PS_lightDirect>& cb_ps_lightDirect );
	void UpdateConstantBuffer( ConstantBuffer<CB_PS_lightDirect>& cb_ps_lightDirect );
public:
	float alphaFactor = 1.0f;
	bool useTexture = true;
private:
	DirectX::XMFLOAT3 ambientColor = XMFLOAT3( 1.0f, 1.0f, 1.0f );
	float ambientStrength = 0.1f;
	DirectX::XMFLOAT3 lightColor = { 1.0f, 1.0f, 1.0f };
	float lightStrength = 2.0f;
	DirectX::XMFLOAT3 specularColor = { 1.0f, 1.0f, 1.0f };
	float specularIntensity = 10.0f;
	float specularPower = 10.0f;
	float constant = 1.0f;
	float linear = 0.045f;
	float quadratic = 0.0075f;
private:
	DirectX::XMFLOAT3 lightColorDirect = { 1.0f, 1.0f, 1.0f };
};

#endif