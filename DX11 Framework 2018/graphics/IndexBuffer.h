#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>

class IndexBuffer
{
private:
	IndexBuffer( const IndexBuffer& rhs ) {}
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;
public:
	IndexBuffer() {}
	ID3D11Buffer* Get() const noexcept
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const noexcept
	{
		return buffer.GetAddressOf();
	}
	UINT BufferSize() const noexcept
	{
		return this->bufferSize;
	}
	HRESULT Initialize( ID3D11Device* device, WORD* data, UINT numIndices )
	{
		if ( buffer.Get() != nullptr )
			buffer.Reset();
		
		this->bufferSize = numIndices;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory( &indexBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof( WORD ) * numIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer( &indexBufferDesc, &indexBufferData, this->buffer.GetAddressOf() );
		return hr;
	}
};