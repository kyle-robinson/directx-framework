#include "Light.h"

bool Light::Initialize( ID3D11Device* device, ID3D11DeviceContext* context,
	ConstantBuffer<CB_VS_matrix>& cb_vs_vertexshader )
{
	if ( !model.Initialize( "res\\models\\light.fbx", device, context, cb_vs_vertexshader ) )
		return false;
	SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	UpdateMatrix();
	return true;
}

void Light::SetConstantBuffer( ConstantBuffer<CB_PS_light>& cb_ps_light )
{
	ambientColor = cb_ps_light.data.ambientLightColor;
	ambientStrength = cb_ps_light.data.ambientLightStrength;
	lightColor = cb_ps_light.data.dynamicLightColor;
	lightStrength = cb_ps_light.data.dynamicLightStrength;
	specularColor = cb_ps_light.data.specularLightColor;
	specularIntensity = cb_ps_light.data.specularLightIntensity;
	specularPower = cb_ps_light.data.specularLightPower;
	constant = cb_ps_light.data.lightConstant;
	linear = cb_ps_light.data.lightLinear;
	quadratic = cb_ps_light.data.lightQuadratic;
	useTexture = cb_ps_light.data.useTexture;
	alphaFactor = cb_ps_light.data.alphaFactor;
}

void Light::UpdateConstantBuffer( ConstantBuffer<CB_PS_light>& cb_ps_light )
{
	cb_ps_light.data.ambientLightColor = ambientColor;
	cb_ps_light.data.ambientLightStrength = ambientStrength;
	cb_ps_light.data.dynamicLightColor = lightColor;
	cb_ps_light.data.dynamicLightStrength = lightStrength;
	cb_ps_light.data.specularLightColor = specularColor;
	cb_ps_light.data.specularLightIntensity = specularIntensity;
	cb_ps_light.data.specularLightPower = specularPower;
	cb_ps_light.data.dynamicLightPosition = GetPositionFloat3();
	cb_ps_light.data.lightConstant = constant;
	cb_ps_light.data.lightLinear = linear;
	cb_ps_light.data.lightQuadratic = quadratic;
	cb_ps_light.data.useTexture = useTexture;
	cb_ps_light.data.alphaFactor = alphaFactor;
}