#include "GameObject.h"

void GameObject::UpdateMatrix()
{
	assert( "UpdateMatrix must be overridden!" && 0 );
}

const XMVECTOR& GameObject::GetPositionVector() const noexcept
{
	return this->posVector;
}

const XMFLOAT3& GameObject::GetPositionFloat3() const noexcept
{
	return this->position;
}

const XMVECTOR& GameObject::GetRotationVector() const noexcept
{
	return this->rotVector;
}

const XMFLOAT3& GameObject::GetRotationFloat3() const noexcept
{
	return this->rotation;
}

void GameObject::SetPosition( const XMVECTOR& pos ) noexcept
{
	XMStoreFloat3( &this->position, pos );
	this->posVector = pos;
	this->UpdateMatrix();
}

void GameObject::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw( this->rotation.x, this->rotation.y, 0.0f );
	this->vec_forward = XMVector3TransformCoord( this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix );
	this->vec_backward = XMVector3TransformCoord( this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix );
	this->vec_left = XMVector3TransformCoord( this->DEFAULT_LEFT_VECTOR, vecRotationMatrix );
	this->vec_right = XMVector3TransformCoord( this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix );

	XMMATRIX vecRotationMatrix_noY = XMMatrixRotationRollPitchYaw( 0.0f, this->rotation.y, 0.0f );
	this->vec_forward_noY = XMVector3TransformCoord( this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix_noY );
	this->vec_backward_noY = XMVector3TransformCoord( this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix_noY );
	this->vec_left_noY = XMVector3TransformCoord( this->DEFAULT_LEFT_VECTOR, vecRotationMatrix_noY );
	this->vec_right_noY = XMVector3TransformCoord( this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix_noY );
}

void GameObject::SetPosition( const XMFLOAT3& pos ) noexcept
{
	this->position = pos;
	this->posVector = XMLoadFloat3( &this->position );
	this->UpdateMatrix();
}

void GameObject::AdjustPosition( const XMVECTOR& pos ) noexcept
{
	this->posVector += pos;
	XMStoreFloat3( &this->position, this->posVector );
	this->UpdateMatrix();
}

void GameObject::AdjustPosition( const XMFLOAT3& pos ) noexcept
{
	this->position.x += pos.x;
	this->position.y += pos.y;
	this->position.z += pos.z;
	this->posVector = XMLoadFloat3( &this->position );
	this->UpdateMatrix();
}

void GameObject::SetRotation( const XMVECTOR& rot ) noexcept
{
	this->rotVector = rot;
	XMStoreFloat3( &this->rotation, rot );
	this->UpdateMatrix();
}

void GameObject::SetRotation( const XMFLOAT3& rot ) noexcept
{
	this->rotation = rot;
	this->rotVector = XMLoadFloat3( &this->rotation );
	this->UpdateMatrix();
}

void GameObject::AdjustRotation( const XMVECTOR& rot ) noexcept
{
	this->rotVector += rot;
	XMStoreFloat3( &this->rotation, this->rotVector );
	this->UpdateMatrix();
}

void GameObject::AdjustRotation( const XMFLOAT3& rot ) noexcept
{
	this->rotation.x += rot.x;
	this->rotation.y += rot.y;
	this->rotation.z += rot.z;
	this->rotVector = XMLoadFloat3( &this->rotation );
	this->UpdateMatrix();
}

void GameObject::SetLookAtPos( XMFLOAT3 lookAtPos ) noexcept
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

const XMVECTOR& GameObject::GetForwardVector( bool omitY ) noexcept
{
	return omitY ? this->vec_forward_noY : this->vec_forward;
}

const XMVECTOR& GameObject::GetBackwardVector( bool omitY ) noexcept
{
	return omitY ? this->vec_backward_noY : this->vec_backward;
}

const XMVECTOR& GameObject::GetLeftVector( bool omitY ) noexcept
{
	return omitY ? this->vec_left_noY : this->vec_left;
}

const XMVECTOR& GameObject::GetRightVector( bool omitY ) noexcept
{
	return omitY ? this->vec_right_noY : this->vec_right;
}