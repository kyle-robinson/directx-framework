#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normals;
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

#endif