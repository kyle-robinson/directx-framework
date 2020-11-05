#include "GameObject.h"

const XMVECTOR& GameObject::GetPositionVector() const noexcept
{
	return posVector;
}

const XMFLOAT3& GameObject::GetPositionFloat3() const noexcept
{
	return position;
}

const XMVECTOR& GameObject::GetRotationVector() const noexcept
{
	return rotVector;
}

const XMFLOAT3& GameObject::GetRotationFloat3() const noexcept
{
	return rotation;
}

void GameObject::SetModelName( const std::string& name ) noexcept
{
	modelName = name;
}

const std::string& GameObject::GetModelName() const noexcept
{
	return modelName;
}

void GameObject::SetPosition( const XMVECTOR& pos ) noexcept
{
	XMStoreFloat3( &position, pos );
	posVector = pos;
	UpdateMatrix();
}

void GameObject::SetPosition( const XMFLOAT3& pos ) noexcept
{
	position = pos;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

void GameObject::AdjustPosition( const XMVECTOR& pos ) noexcept
{
	posVector += pos;
	XMStoreFloat3( &position, posVector );
	UpdateMatrix();
}

void GameObject::AdjustPosition( const XMFLOAT3& pos ) noexcept
{
	position.x += pos.x;
	position.y += pos.y;
	position.z += pos.z;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

void GameObject::SetRotation( const XMVECTOR& rot ) noexcept
{
	rotVector = rot;
	XMStoreFloat3( &rotation, rot );
	UpdateMatrix();
}

void GameObject::SetRotation( const XMFLOAT3& rot ) noexcept
{
	rotation = rot;
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

void GameObject::AdjustRotation( const XMVECTOR& rot ) noexcept
{
	rotVector += rot;
	XMStoreFloat3( &rotation, rotVector );
	UpdateMatrix();
}

void GameObject::AdjustRotation( const XMFLOAT3& rot ) noexcept
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

void GameObject::SetScale( float xScale, float yScale, float zScale )
{
	scale.x = xScale;
	scale.y = yScale;
	scale.z = zScale;
	UpdateMatrix();
}

void GameObject::UpdateMatrix()
{
	assert( "UpdateMatrix must be overridden!" && 0 );
}