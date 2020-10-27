#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"
using namespace DirectX;

class Camera : public GameObject
{
public:
	Camera();
	void SetProjectionValues( float fovDegrees, float aspectRatio, float nearZ, float farZ );
public:
	const XMMATRIX& GetViewMatrix() const noexcept;
	const XMMATRIX& GetProjectionMatrix() const noexcept;
private:
	void UpdateMatrix() override;
	XMMATRIX view, projection;
};

#endif