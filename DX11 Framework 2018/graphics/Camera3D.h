#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject3D.h"
using namespace DirectX;

class Camera3D : public GameObject3D
{
public:
	Camera3D();
	void SetProjectionValues( float fovDegrees, float aspectRatio, float nearZ, float farZ );

	const XMMATRIX& GetViewMatrix() const noexcept;
	const XMMATRIX& GetProjectionMatrix() const noexcept;

	const float& GetCameraSpeed() const noexcept;
	void SetCameraSpeed( float newSpeed ) noexcept;
	void UpdateCameraSpeed( float updateSpeed ) noexcept;

	const float& GetFoVDegrees() const noexcept;
	const float& GetNearZ() const noexcept;
	const float& GetFarZ() const noexcept;

	void ResetPosition() noexcept;
	void ResetRotation() noexcept;
	void ResetProjection( float aspectRatio ) noexcept;
private:
	void UpdateMatrix() override;
	XMMATRIX view, projection;
	float cameraSpeed, fovDegrees;
	float nearZ, farZ;
};

#endif