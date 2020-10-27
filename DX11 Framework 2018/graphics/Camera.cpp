#include "Camera.h"

Camera::Camera()
{
	this->position = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	this->posVector = XMLoadFloat3( &this->position );
	this->rotation = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	this->rotVector = XMLoadFloat3( &this->rotation );
	this->UpdateMatrix();
}

void Camera::SetProjectionValues( float fovDegrees, float aspectRatio, float nearZ, float farZ )
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projection = XMMatrixPerspectiveFovLH( fovRadians, aspectRatio, nearZ, farZ );
}

const XMMATRIX& Camera::GetViewMatrix() const noexcept
{
	return this->view;
}

const XMMATRIX& Camera::GetProjectionMatrix() const noexcept
{
	return this->projection;
}

void Camera::UpdateMatrix()
{
	// update camera target
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYaw( this->rotation.x, this->rotation.y, this->rotation.z );
	XMVECTOR camTarget = XMVector3TransformCoord( this->DEFAULT_FORWARD_VECTOR, cameraRotation );
	camTarget += this->posVector;

	// calculate up direction vector based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord( this->DEFAULT_UP_VECTOR, cameraRotation );
	this->view = XMMatrixLookAtLH( this->posVector, camTarget, upDir );

	this->UpdateDirectionVectors();
}