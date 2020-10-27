#include "Mesh.h"

Mesh::Mesh( ID3D11Device* device,
	ID3D11DeviceContext* context,
	std::vector<Vertex>& vertices,
	std::vector<WORD>& indices,
	std::vector<Texture>& textures,
	const DirectX::XMMATRIX& transformMatrix )
{
	try
	{
		this->context = context;
		this->textures = textures;
		this->transformMatrix = transformMatrix;

		HRESULT hr = this->vertexBuffer.Initialize( device, vertices.data(), vertices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize vertex buffer for mesh!" );

		hr = this->indexBuffer.Initialize( device, indices.data(), indices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize index buffer for mesh!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return;
	}
}

const DirectX::XMMATRIX& Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}

Mesh::Mesh( const Mesh& mesh )
{
	this->context = mesh.context;
	this->vertexBuffer = mesh.vertexBuffer;
	this->indexBuffer = mesh.indexBuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
}

void Mesh::Draw()
{
	for ( int i = 0; i < textures.size(); i++ )
	{
		if ( textures[i].GetType() == aiTextureType_DIFFUSE )
		{
			this->context->PSSetShaderResources( 0, 1, textures[i].GetTextureResourceViewAddress() );
			break;
		}
	}

	UINT offset = 0;
	this->context->IASetVertexBuffers( 0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset );
	this->context->IASetIndexBuffer( this->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );
	this->context->DrawIndexed( this->indexBuffer.IndexCount(), 0, 0 );
}