#include "Camera.h"

Camera::Camera() :
	position( XMFLOAT3( 0.0f, 0.0f, 0.0f ) ), posVector( XMLoadFloat3( &this->position ) ),
	rotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) ), rotVector( XMLoadFloat3( &this->rotation ) )
{
	this->UpdateViewMatrix();
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

const XMVECTOR& Camera::GetPositionVector() const noexcept
{
	return this->posVector;
}

const XMFLOAT3& Camera::GetPositionFloat3() const noexcept
{
	return this->position;
}

const XMVECTOR& Camera::GetRotationVector() const noexcept
{
	return this->rotVector;
}

const XMFLOAT3& Camera::GetRotationFloat3() const noexcept
{
	return this->rotation;
}

void Camera::SetPosition( const XMVECTOR& pos ) noexcept
{
	XMStoreFloat3( &this->position, pos );
	this->posVector = pos;
	this->UpdateViewMatrix();
}

void Camera::SetPosition( const XMFLOAT3& pos ) noexcept
{
	this->position = pos;
	this->posVector = XMLoadFloat3( &this->position );
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition( const XMVECTOR& pos ) noexcept
{
	this->posVector += pos;
	XMStoreFloat3( &this->position, this->posVector );
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition( const XMFLOAT3& pos ) noexcept
{
	this->position.x += pos.x;
	this->position.y += pos.y;
	this->position.z += pos.z;
	this->posVector = XMLoadFloat3( &this->position );
	this->UpdateViewMatrix();
}

void Camera::SetRotation( const XMVECTOR& rot ) noexcept
{
	this->rotVector = rot;
	XMStoreFloat3( &this->rotation, rot );
	this->UpdateViewMatrix();
}

void Camera::SetRotation( const XMFLOAT3& rot ) noexcept
{
	this->rotation = rot;
	this->rotVector = XMLoadFloat3( &this->rotation );
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation( const XMVECTOR& rot ) noexcept
{
	this->rotVector += rot;
	XMStoreFloat3( &this->rotation, this->rotVector );
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation( const XMFLOAT3& rot ) noexcept
{
	this->rotation.x += rot.x;
	this->rotation.y += rot.y;
	this->rotation.z += rot.z;
	this->rotVector = XMLoadFloat3( &this->rotation );
	this->UpdateViewMatrix();
}

void Camera::SetLookAtPos( XMFLOAT3 lookAtPos ) noexcept
{
	if ( lookAtPos.x == this->position.x &&
		 lookAtPos.y == this->position.y &&
		 lookAtPos.z == this->position.z ) return;

	lookAtPos.x = this->position.x - lookAtPos.x;
	lookAtPos.y = this->position.y - lookAtPos.y;
	lookAtPos.z = this->position.z - lookAtPos.z;

	float pitch = 0.0f;
	if ( lookAtPos.y != 0.0f )
	{
		const float distance = sqrt( lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z );
		pitch = atan( lookAtPos.y / distance );
	}

	float yaw = 0.0f;
	if ( lookAtPos.x != 0.0f )
		yaw = atan( lookAtPos.x / lookAtPos.z );
	if ( lookAtPos.z > 0.0f )
		yaw += XM_PI;

	this->SetRotation( XMFLOAT3( pitch, yaw, 0.0f ) );
}

const XMVECTOR& Camera::GetForwardVector() noexcept
{
	return this->vec_forward;
}

const XMVECTOR& Camera::GetBackwardVector() noexcept
{
	return this->vec_backward;
}

const XMVECTOR& Camera::GetLeftVector() noexcept
{
	return this->vec_left;
}

const XMVECTOR& Camera::GetRightVector() noexcept
{
	return this->vec_right;
}

void Camera::UpdateViewMatrix()
{
	// update camera target
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYaw( this->rotation.x, this->rotation.y, this->rotation.z );
	XMVECTOR camTarget = XMVector3TransformCoord( this->DEFAULT_FORWARD_VECTOR, cameraRotation );
	camTarget += this->posVector;

	// calculate up direction vector based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord( this->DEFAULT_UP_VECTOR, cameraRotation );
	this->view = XMMatrixLookAtLH( this->posVector, camTarget, upDir );

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw( 0.0f, this->rotation.y, 0.0f );
	this->vec_forward = XMVector3TransformCoord( this->DEFAULT_FORWARD_VECTOR, rotationMatrix );
	this->vec_backward = XMVector3TransformCoord( this->DEFAULT_BACKWARD_VECTOR, rotationMatrix );
	this->vec_left = XMVector3TransformCoord( this->DEFAULT_LEFT_VECTOR, rotationMatrix );
	this->vec_right = XMVector3TransformCoord( this->DEFAULT_RIGHT_VECTOR, rotationMatrix );
}