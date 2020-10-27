#pragma once
#ifndef RENDERABLEGAMEOBJECT_H
#define RENDERABLEGAMEOBJECT_H

#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	bool Initialize(
		const std::string& filePath,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		ConstantBuffer<CB_VS_matrix>& cb_vs_vertexshader );
	void Draw( const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix );
protected:
	Model model;
	void UpdateMatrix() override;
	XMMATRIX worldMatrix = XMMatrixIdentity();
};

#endif