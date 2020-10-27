#include "Light.h"

bool Light::Initialize( ID3D11Device* device, ID3D11DeviceContext* context,
	ConstantBuffer<CB_VS_vertexShader>& cb_vs_vertexshader )
{
	if ( !model.Initialize( "res\\models\\light.fbx", device, context, cb_vs_vertexshader ) )
		return false;
	this->SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	this->SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	this->UpdateMatrix();
	return true;
}