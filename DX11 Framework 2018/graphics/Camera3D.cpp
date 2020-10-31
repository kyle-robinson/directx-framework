#include "Camera3D.h"

Camera3D::Camera3D()
{
	position = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	posVector = XMLoadFloat3( &position );
	rotation = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

void Camera3D::SetProjectionValues( float fovDegrees, float aspectRatio, float nearZ, float farZ )
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	projection = XMMatrixPerspectiveFovLH( fovRadians, aspectRatio, nearZ, farZ );
}

const XMMATRIX& Camera3D::GetViewMatrix() const noexcept
{
	return view;
}

const XMMATRIX& Camera3D::GetProjectionMatrix() const noexcept
{
	return projection;
}

void Camera3D::UpdateMatrix()
{
	// update camera target
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, rotation.z );
	XMVECTOR camTarget = XMVector3TransformCoord( DEFAULT_FORWARD_VECTOR, cameraRotation );
	camTarget += posVector;

	// calculate up direction vector based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord( DEFAULT_UP_VECTOR, cameraRotation );
	view = XMMatrixLookAtLH( posVector, camTarget, upDir );

	UpdateDirectionVectors();
}