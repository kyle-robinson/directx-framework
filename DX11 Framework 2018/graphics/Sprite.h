#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include "GameObject2D.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Vertex.h"

class Sprite : public GameObject2D
{
public:
	bool Initialize( ID3D11Device* device, ID3D11DeviceContext* context,
		float width, float height, std::string spritePath,
		ConstantBuffer<CB_VS_matrix_2D>& cb_vs_vertexshader_2d );
	void Draw( XMMATRIX orthoMatrix );
	float GetWidth() const noexcept;
	float GetHeight() const noexcept;
private:
	void UpdateMatrix() override;
	std::unique_ptr<Texture> texture;
	ID3D11DeviceContext* context = nullptr;
private:
	XMMATRIX worldMatrix = XMMatrixIdentity();
	ConstantBuffer<CB_VS_matrix_2D>* cb_vs_matrix_2d = nullptr;
	VertexBuffer<Vertex2D> vertices;
	IndexBuffer indices;
};

#endif