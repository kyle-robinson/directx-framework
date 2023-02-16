#include "Cube.h"

Vertex3D verticesCube_PosTexNrm[] = {
        { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f,  1.0f,  0.0f } }, // +Y (top face)
        { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f,  1.0f,  0.0f } },
        { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f,  1.0f,  0.0f } },
        { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f }, { 0.0f,  1.0f,  0.0f } },
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f,  0.0f } }, // -Y (bottom face)
        { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0f, -1.0f,  0.0f } },
        { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f,  0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, -1.0f,  0.0f } },
        { {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } }, // +X (right face)
        { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } },
        { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f,  0.0f,  0.0f } },
        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0f,  0.0f,  0.0f } },
        { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } }, // -X (left face)
        { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } },
        { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 1.0f,  0.0f,  0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 1.0f,  0.0f,  0.0f } },
        { { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f,  0.0f,  1.0f } }, // +Z (front face)
        { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0f,  0.0f,  1.0f } },
        { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f,  0.0f,  1.0f } },
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, { 0.0f,  0.0f,  1.0f } },
        { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f,  0.0f, -1.0f } }, // -Z (back face)
        { { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f,  0.0f, -1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f,  0.0f, -1.0f } },
        { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f,  0.0f, -1.0f } }
};

WORD indicesLightCube[] =
{
    23, 22, 20,
    22, 21, 20,
    19, 18, 16,
    18, 17, 16,
    15, 14, 12,
    14, 13, 12,
    11, 10, 8,
    10, 9, 8,
    7, 6, 4,
    6, 5, 4,
    3, 2, 0,
    2, 1, 0
};

Cube::Cube( ID3D11DeviceContext* context, ID3D11Device* device )
{
    Initialize( context, device );
}

bool Cube::Initialize( ID3D11DeviceContext* context, ID3D11Device* device )
{
    this->context = context;

    try
    {
        HRESULT hr = vb_cube.Initialize( device, verticesCube_PosTexNrm, ARRAYSIZE( verticesCube_PosTexNrm ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
        hr = ib_cube.Initialize( device, indicesLightCube, ARRAYSIZE( indicesLightCube ) );
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