#pragma once
#include <Windows.h>

class Vector3D
{
public:
	float x, y, z;
public:
	Vector3D();
	Vector3D( float x, float y, float z = 0 );
	Vector3D( const Vector3D &vec );
public:
	Vector3D operator+( const Vector3D &vec );
	Vector3D& operator+=( const Vector3D &vec );
	Vector3D operator-( const Vector3D &vec );
	Vector3D& operator-=( const Vector3D &vec );
	Vector3D operator*( float value );
	Vector3D& operator*=( float value );
	Vector3D operator/( float value );
	Vector3D& operator/=( float value );
	Vector3D& operator=( const Vector3D &vec );
public:
	float DotProduct( const Vector3D &vec );
	Vector3D CrossProduct( const Vector3D &vec );
	Vector3D Normalization();
public:
	float Square();
	float Distance( const Vector3D &vec );
	float Magnitude();
public:
	float ShowX() const noexcept;
	float ShowY() const noexcept;
	float ShowZ() const noexcept;
	void Display() noexcept;
};