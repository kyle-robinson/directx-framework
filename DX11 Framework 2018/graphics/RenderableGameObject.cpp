#include "RenderableGameObject.h"

bool RenderableGameObject::Initialize(
	const std::string& filePath,
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	ConstantBuffer<CB_VS_vertexShader>& cb_vs_vertexshader )
{
	if ( !model.Initialize( filePath, device, context, cb_vs_vertexshader ) )
		return false;

	this->SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	this->SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	this->UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw( const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix )
{
	model.Draw( this->worldMatrix, viewMatrix, projectionMatrix );
}

void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw( this->rotation.x, this->rotation.y, this->rotation.z ) *
		XMMatrixTranslation( this->position.x, this->position.y, this->position.z );
	this->UpdateDirectionVectors();
}