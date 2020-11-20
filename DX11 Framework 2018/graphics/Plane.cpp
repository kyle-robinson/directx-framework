#include "Plane.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"

/// NORMAL PLANE
bool Plane::Initialize( ID3D11DeviceContext* context, ID3D11Device* device )
{
    this->context = context;

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

    return false;
}

void Plane::Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ID3D11ShaderResourceView* texture ) noexcept
{
    UINT offset = 0;
    context->IASetVertexBuffers( 0, 1, vb_plane.GetAddressOf(), vb_plane.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_plane.Get(), DXGI_FORMAT_R16_UINT, 0 );
    context->PSSetShaderResources( 0, 1, &texture );
    //cb_vs_matrix.data.worldMatrix = XMMatrixIdentity();
    //if ( !cb_vs_matrix.ApplyChanges() ) return;
    //context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
    context->DrawIndexed( ib_plane.IndexCount(), 0, 0 );
}

/// INSTANCED PLANE
bool PlaneInstanced::InitializeInstanced( ID3D11DeviceContext* context, ID3D11Device* device, int planeAmount )
{
    this->context = context;
    this->planeAmount = planeAmount;

    try
    {
        HRESULT hr = vb_plane.Initialize( device, VTX::verticesQuad, ARRAYSIZE( VTX::verticesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create instanced quad vertex buffer!" );
        hr = ib_plane.Initialize( device, IDX::indicesQuad, ARRAYSIZE( IDX::indicesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create instanced quad index buffer!" );
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

void PlaneInstanced::DrawInstanced( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ConstantBuffer<CB_PS_light>& cb_ps_light,
    ID3D11ShaderResourceView* texture ) noexcept
{
	UINT offset = 0;
    context->IASetVertexBuffers( 0, 1, vb_plane.GetAddressOf(), vb_plane.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_plane.Get(), DXGI_FORMAT_R16_UINT, 0 );
    context->PSSetShaderResources( 0, 1, &texture );
    cb_ps_light.data.useQuad = true;
    if ( !cb_ps_light.ApplyChanges() ) return;
    context->PSSetConstantBuffers( 2, 1, cb_ps_light.GetAddressOf() );
    for ( int i = 0; i < planeAmount; i++ )
    {
        cb_vs_matrix.data.worldMatrix = XMLoadFloat4x4( &worldMatrices[i] );
        if ( !cb_vs_matrix.ApplyChanges() ) return;
        context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
        context->DrawIndexed( ib_plane.IndexCount(), 0, 0 );
    }
}

void PlaneInstanced::UpdateInstanced( int tileSize, int tileOffset, int worldOffsetX, int worldOffsetY ) noexcept
{
    int count = 0;
    for ( int row = 0; row < sqrt( planeAmount ); row++ )
    {
        for ( int col = 0; col < sqrt( planeAmount ); col++ )
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

/// FULLSCREEN PLANE
bool PlaneFullscreen::Initialize( ID3D11DeviceContext* context, ID3D11Device* device )
{
    this->context = context;

    try
    {
        HRESULT hr = vb_full.Initialize( device, VTX::verticesFullscreen, ARRAYSIZE( VTX::verticesFullscreen ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen quad vertex buffer!" );
        hr = ib_full.Initialize( device, IDX::indicesFullscreen, ARRAYSIZE( IDX::indicesFullscreen ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create fullscreen quad index buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    
    return true;
}

void PlaneFullscreen::SetupBuffers( VertexShader& vs_full, PixelShader& ps_full, ConstantBuffer<CB_VS_fullscreen>& cb_vs_full, bool& multiView ) noexcept 
{
    UINT offset = 0;
    Shaders::BindShaders( context, vs_full, ps_full );    
    context->IASetVertexBuffers( 0, 1, vb_full.GetAddressOf(), vb_full.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_full.Get(), DXGI_FORMAT_R16_UINT, 0 );
    cb_vs_full.data.multiView = multiView;
    if ( !cb_vs_full.ApplyChanges() ) return;
    context->VSSetConstantBuffers( 0, 1, cb_vs_full.GetAddressOf() );
}