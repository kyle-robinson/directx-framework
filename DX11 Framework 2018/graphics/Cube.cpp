#include "Cube.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"

bool Cube::Initialize( ID3D11DeviceContext* context, ID3D11Device* device, int cubeAmount )
{
    this->context = context;
    this->cubeAmount = cubeAmount;

    try
    {
        HRESULT hr = vb_cube.Initialize( device, VTX::verticesCube_PosTexNrm, ARRAYSIZE( VTX::verticesCube_PosTexNrm ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
        hr = ib_cube.Initialize( device, IDX::indicesLightCube, ARRAYSIZE( IDX::indicesLightCube ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }

    for ( int i = 0; i < cubeAmount; i++ )
    {
        XMFLOAT4X4 worldMatrix;
        XMStoreFloat4x4( &worldMatrix, XMMatrixIdentity() );
        worldMatrices.push_back( worldMatrix );
    }

    return true;
}

void Cube::Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ID3D11ShaderResourceView* texture ) noexcept
{
	UINT offset = 0;
    context->IASetVertexBuffers( 0, 1, vb_cube.GetAddressOf(), vb_cube.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_cube.Get(), DXGI_FORMAT_R16_UINT, 0 );
    context->PSSetShaderResources( 0, 1, &texture );
    for ( int i = 0; i < worldMatrices.size(); i++ )
    {
        XMMATRIX worldMatrix = XMLoadFloat4x4( &worldMatrices[i] );
        cb_vs_matrix.data.worldMatrix = worldMatrix;
        if ( !cb_vs_matrix.ApplyChanges() ) return;
        context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
        context->DrawIndexed( ib_cube.IndexCount(), 0, 0 );
    }
}

void Cube::Update() noexcept
{
    static float timer = 0.0f;
    static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount64();
	if ( dwTimeStart == 0 ) dwTimeStart = dwTimeCur;
	timer = ( dwTimeCur - dwTimeStart ) / 1000.0f;

    XMStoreFloat4x4( &worldMatrices[0],
        XMMatrixScaling( 2.0f, 2.0f, 2.0f ) *
        XMMatrixRotationZ( timer * 1.5f ) *
        XMMatrixTranslation( 0.0f, 10.0f, 15.0f )
    );
    XMStoreFloat4x4( &worldMatrices[1],
        XMMatrixScaling( 2.0f, 2.0f, 2.0f ) *
        XMMatrixTranslation( 10.0f, 10.0f, 0.0f ) *
        XMMatrixRotationY( timer )
    );
    XMStoreFloat4x4( &worldMatrices[2],
        XMMatrixScaling( 2.0f, 2.0f, 2.0f ) *
        XMMatrixTranslation( 1.0f, 10.0f, 0.0f ) *
        XMMatrixRotationZ( timer * 1.5f ) *
        XMMatrixTranslation( 0.0f, 7.5f, 0.0f )
    );
}