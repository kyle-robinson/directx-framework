#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include <DirectXMath.h>

struct Vertex3D
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normals;
};

struct Vertex2D
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};

struct Vertex_Pos_Col
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

struct Vertex_Pos_Tex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texCoord;
};

struct Vertex_Pos_Nrm
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
};

struct Vertex_Pos
{
	DirectX::XMFLOAT2 position;
};

#endif