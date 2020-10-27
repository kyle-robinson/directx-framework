#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Model.h"

class GameObject
{
public:
	const XMVECTOR& GetPositionVector() const noexcept;
	const XMFLOAT3& GetPositionFloat3() const noexcept;
	const XMVECTOR& GetRotationVector() const noexcept;
	const XMFLOAT3& GetRotationFloat3() const noexcept;
public:
	void SetPosition( const XMVECTOR& pos ) noexcept;
	void SetPosition( const XMFLOAT3& pos ) noexcept;
	void AdjustPosition( const XMVECTOR& pos ) noexcept;
	void AdjustPosition( const XMFLOAT3& pos ) noexcept;
	void SetRotation( const XMVECTOR& rot ) noexcept;
	void SetRotation( const XMFLOAT3& rot ) noexcept;
	void AdjustRotation( const XMVECTOR& rot ) noexcept;
	void AdjustRotation( const XMFLOAT3& rot ) noexcept;
	void SetLookAtPos( XMFLOAT3 lookAtPos ) noexcept;
public:
	const XMVECTOR& GetForwardVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetBackwardVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetLeftVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetRightVector( bool omitY = false ) noexcept;
protected:
	virtual void UpdateMatrix();
	void UpdateDirectionVectors();
	XMVECTOR posVector, rotVector;
	XMFLOAT3 position, rotation;
protected:
	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
protected:
	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
	XMVECTOR vec_forward_noY;
	XMVECTOR vec_left_noY;
	XMVECTOR vec_right_noY;
	XMVECTOR vec_backward_noY;
};

#endif