#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	void SetProjectionValues( float fovDegrees, float aspectRatio, float nearZ, float farZ );
public:
	const XMMATRIX& GetViewMatrix() const noexcept;
	const XMMATRIX& GetProjectionMatrix() const noexcept;
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
	const XMVECTOR& GetForwardVector() noexcept;
	const XMVECTOR& GetBackwardVector() noexcept;
	const XMVECTOR& GetLeftVector() noexcept;
	const XMVECTOR& GetRightVector() noexcept;
private:
	void UpdateViewMatrix();
	XMMATRIX view, projection;
	XMVECTOR posVector, rotVector;
	XMFLOAT3 position, rotation;
private:
	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
private:
	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
};