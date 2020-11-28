#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "Shaders.h"
#include "RenderableGameObject.h"

class Plane : public RenderableGameObject
{
public:
	bool Initialize( ID3D11DeviceContext* context, ID3D11Device* device );
	void Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ID3D11ShaderResourceView* texture ) noexcept;
private:
	ID3D11DeviceContext* context;
	VertexBuffer<Vertex3D> vb_plane;
	IndexBuffer ib_plane;
};

class PlaneInstanced : public RenderableGameObject
{
public:
	bool InitializeInstanced( ID3D11DeviceContext* context, ID3D11Device* device, int planeAmount );
	void DrawInstanced( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ConstantBuffer<CB_PS_light>& cb_ps_light,
		ID3D11ShaderResourceView* texture ) noexcept;
	void UpdateInstanced( int tileSize, int tileOffset, int worldOffsetX, int worldOffsetY ) noexcept;
private:
	ID3D11DeviceContext* context;
	std::vector<XMFLOAT4X4> worldMatrices;
	VertexBuffer<Vertex3D> vb_plane;
	IndexBuffer ib_plane;
	int planeAmount;
};

class PlaneFullscreen : public RenderableGameObject
{
public:
	bool Initialize( ID3D11DeviceContext* context, ID3D11Device* device );
	void SetupBuffers( VertexShader& vs_full, PixelShader& ps_full, ConstantBuffer<CB_VS_fullscreen>& cb_vs_full, bool& multiView ) noexcept;
public:
	IndexBuffer ib_full;
private:
	ID3D11DeviceContext* context;
	VertexBuffer<Vertex_Pos> vb_full;
};

#endif