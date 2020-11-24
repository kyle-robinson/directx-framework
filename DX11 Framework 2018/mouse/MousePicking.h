#pragma once
#ifndef MOUSEPICKING_H
#define MOUSEPICKING_H

#include <memory>
#include <DirectXMath.h>
using namespace DirectX;

class Graphics;
class Camera3D;

class MousePicking
{
public:
	void Initialize( XMMATRIX viewMatrix, XMMATRIX projectionMatrix, int width, int height );
	void TestIntersection( int mouseX, int mouseY );
	void UpdateMatrices( XMMATRIX viewMatrix, XMMATRIX projectionMatrix );
	bool RaySphereIntersect( XMFLOAT3 objectPos );
private:
	XMMATRIX viewMatrix, projectionMatrix;
	XMVECTOR rayDirection, rayOrigin;
	int width, height;
};

#endif