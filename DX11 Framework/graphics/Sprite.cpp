#include "Sprite.h"
#include <dxtk/WICTextureLoader.h>

bool Sprite::Initialize( ID3D11Device* device, ID3D11DeviceContext* context,
	float width, float height, std::string spritePath,
	ConstantBuffer<CB_VS_matrix_2D>& cb_vs_matrix_2d )
{
	this->context = context;
	if ( context == nullptr ) return false;

	this->cb_vs_matrix_2d = &cb_vs_matrix_2d;
	texture = std::make_unique<Texture>( device, spritePath, aiTextureType_DIFFUSE );

	std::vector<Vertex2D> vertexData =
	{
		{ { -0.5f, -0.5f, 0.0f },{ 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f } },
		{ { -0.5f,  0.5f, 0.0f },{ 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, 0.0f },{ 1.0f, 1.0f } },
	};

	std::vector<WORD> indexData =
	{
		0, 1, 2,
		2, 1, 3
	};

	HRESULT hr = vertices.Initialize( device, vertexData.data(), vertexData.size() );
	COM_ERROR_IF_FAILED( hr, "Failed to initialize vertices for sprite!" );

	hr = indices.Initialize( device, indexData.data(), indexData.size() );
	COM_ERROR_IF_FAILED( hr, "Failed to initialize indices for sprite!" );

	SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	SetScale( width, height );

	return true;
}

void Sprite::Draw( XMMATRIX orthoMatrix )
{
	XMMATRIX wvpMatrix = this->worldMatrix * orthoMatrix;
	this->context->VSSetConstantBuffers( 0, 1, cb_vs_matrix_2d->GetAddressOf() );
	cb_vs_matrix_2d->data.wvpMatrix = wvpMatrix;
	cb_vs_matrix_2d->ApplyChanges();
	this->context->PSSetShaderResources( 0, 1, this->texture->GetTextureResourceViewAddress() );

	const UINT offsets = 0;
	this->context->IASetVertexBuffers( 0, 1, this->vertices.GetAddressOf(), this->vertices.StridePtr(), &offsets );
	this->context->IASetIndexBuffer( this->indices.Get(), DXGI_FORMAT_R16_UINT, 0 );
	this->context->DrawIndexed( this->indices.IndexCount(), 0, 0 );
}

float Sprite::GetWidth() const noexcept
{
	return this->scale.x;
}

float Sprite::GetHeight() const noexcept
{
	return this->scale.y;
}

void Sprite::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling( this->scale.x, this->scale.y, 1.0f ) *
		XMMatrixRotationRollPitchYaw( this->rotation.x, this->rotation.y, this->rotation.z ) *
		XMMatrixTranslation( this->position.x + this->scale.x / 2.0f, this->position.y + this->scale.y / 2.0f, this->position.z );
}