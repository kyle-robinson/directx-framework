#include "Cube.h"
#include "ObjectIndices.h"
#include "ObjectVertices.h"

Cube::Cube( ID3D11DeviceContext* context, ID3D11Device* device )
{
    Initialize( context, device );
}

bool Cube::Initialize( ID3D11DeviceContext* context, ID3D11Device* device )
{
    this->context = context;

    try
    {
        HRESULT hr = vb_cube.Initialize( device, VTX::verticesCube_PosTexNrm, ARRAYSIZE( VTX::verticesCube_PosTexNrm ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
        hr = ib_cube.Initialize( device, IDX::indicesLightCube, ARRAYSIZE( IDX::indicesLightCube ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

        SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	    SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
        SetScale( 1.0f, 1.0f, 1.0f );
	    UpdateMatrix();
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    
    return true;
}

void Cube::Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ID3D11ShaderResourceView* texture ) noexcept
{
	UINT offset = 0;
    context->IASetVertexBuffers( 0, 1, vb_cube.GetAddressOf(), vb_cube.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_cube.Get(), DXGI_FORMAT_R16_UINT, 0 );
    context->PSSetShaderResources( 0, 1, &texture );
    cb_vs_matrix.data.worldMatrix = XMMatrixIdentity() * worldMatrix;
    if ( !cb_vs_matrix.ApplyChanges() ) return;
    context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
    context->DrawIndexed( ib_cube.IndexCount(), 0, 0 );
}