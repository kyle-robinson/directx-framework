#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "RenderableGameObject.h"

class Plane : public RenderableGameObject
{
public:
	bool Initialize( ID3D11DeviceContext* context, ID3D11Device* device, int planeAmount );
	void Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ConstantBuffer<CB_PS_light>& cb_ps_light,
		ID3D11ShaderResourceView* texture ) noexcept;
	void Update() noexcept;
private:
	ID3D11DeviceContext* context;
	VertexBuffer<Vertex3D> vb_plane;
	IndexBuffer ib_plane;
	int planeAmount;
	std::vector<XMFLOAT4X4> worldMatrices;
};

#endif