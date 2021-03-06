#pragma once
#ifndef CUBE_H
#define CUBE_H

#include "RenderableGameObject.h"

class Cube : public RenderableGameObject
{
public:
	Cube( ID3D11DeviceContext* context, ID3D11Device* device );
	bool Initialize( ID3D11DeviceContext* context, ID3D11Device* device );
	void Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, ID3D11ShaderResourceView* texture ) noexcept;
private:
	ID3D11DeviceContext* context;
	VertexBuffer<Vertex3D> vb_cube;
	IndexBuffer ib_cube;
};

#endif