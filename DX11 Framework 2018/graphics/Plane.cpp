#include "Plane.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"

bool Plane::Initialize( ID3D11DeviceContext* context, ID3D11Device* device, int planeAmount )
{
    this->context = context;
    this->planeAmount = planeAmount;

    try
    {
        HRESULT hr = vb_plane.Initialize( device, VTX::verticesQuad, ARRAYSIZE( VTX::verticesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad vertex buffer!" );
        hr = ib_plane.Initialize( device, IDX::indicesQuad, ARRAYSIZE( IDX::indicesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad index buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }

    for ( int i = 0; i < planeAmount; i++ )
    {
        XMFLOAT4X4 worldMatrix;
        XMStoreFloat4x4( &worldMatrix, XMMatrixIdentity() );
        worldMatrices.push_back( worldMatrix );
    }

    return true;
}

void Plane::Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ConstantBuffer<CB_PS_light>& cb_ps_light,
    ID3D11ShaderResourceView* texture ) noexcept
{
	UINT offset = 0;
    context->IASetVertexBuffers( 0, 1, vb_plane.GetAddressOf(), vb_plane.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_plane.Get(), DXGI_FORMAT_R16_UINT, 0 );
    context->PSSetShaderResources( 0, 1, &texture );
    cb_ps_light.data.useQuad = true;
    if ( !cb_ps_light.ApplyChanges() ) return;
    context->PSSetConstantBuffers( 2, 1, cb_ps_light.GetAddressOf() );
    for ( int i = 0; i < worldMatrices.size(); i++ )
    {
        cb_vs_matrix.data.worldMatrix = XMLoadFloat4x4( &worldMatrices[i] );
        if ( !cb_vs_matrix.ApplyChanges() ) return;
        context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
        context->DrawIndexed( ib_plane.IndexCount(), 0, 0 );
    }
}

void Plane::Update() noexcept
{
    int count = 0;
    static int tileSize = 5;
    static int tileOffset = 6;
    static int worldOffsetX = 8;
    static int worldOffsetY = 60;
    for ( int row = 0; row < 20; row++ )
    {
        for ( int col = 0; col < 20; col++ )
        {
            XMStoreFloat4x4( &worldMatrices[count],
                XMMatrixScaling( tileSize, tileSize, 0.0f ) *
                XMMatrixRotationX( XMConvertToRadians( 90.0f ) ) *
                XMMatrixTranslation(
                    ( row * tileOffset * tileSize ) - ( worldOffsetX + worldOffsetY * tileSize ),
                    4.7f, ( col * tileOffset * tileSize ) - worldOffsetY * tileSize ) );
            count++;
        }
    }
}