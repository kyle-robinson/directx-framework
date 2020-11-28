#include "Vector3D.h"
#include <sstream>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <debugapi.h>

Vector3D::Vector3D() : x( 0 ), y( 0 ), z( 0 ) { }

Vector3D::Vector3D( float x, float y, float z ) : x( x ), y( y ), z( z ) { }

Vector3D::Vector3D( const Vector3D &vec ) : x( vec.x ), y( vec.y ), z( vec.z ) { }

Vector3D Vector3D::operator+( const Vector3D &vec )
{
	return Vector3D( x + vec.x, y + vec.y, z + vec.z );
}

Vector3D& Vector3D::operator+=( const Vector3D &vec )
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

Vector3D Vector3D::operator-( const Vector3D &vec )
{
	return Vector3D( x - vec.x, y - vec.y, z - vec.z );
}

Vector3D& Vector3D::operator-=( const Vector3D &vec )
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

Vector3D Vector3D::operator*( float value )
{
	return Vector3D( x * value, y * value, z * value );
}

Vector3D& Vector3D::operator*=( float value )
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

Vector3D Vector3D::operator/( float value )
{
	assert( value != 0, "Vector Division: Value was equal to 0!" );
	return Vector3D( x / value, y / value, z / value );
}

Vector3D& Vector3D::operator/=( float value )
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

Vector3D& Vector3D::operator=( const Vector3D &vec )
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	return Vector3D( x, y, z );
}

float Vector3D::DotProduct( const Vector3D &vec )
{
	return ( x * vec.x + y + vec.y + x * vec.z );
}

Vector3D Vector3D::CrossProduct( const Vector3D &vec )
{
	return Vector3D(
		y * vec.z - z * vec.y,
		z * vec.x - x * vec.z,
		x * vec.y - y * vec.x
	);
}

Vector3D Vector3D::Normalization()
{
	float magnitude = this->Magnitude();
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
	return Vector3D( x, y, z );
}

float Vector3D::Square()
{
	return pow( x, x ) + pow( y, y ) + pow( z, z );
}

float Vector3D::Distance( const Vector3D &vec )
{
	return sqrt(
		pow( x - ( -vec.x ), x - ( -vec.x ) ) +
		pow( y - ( -vec.y ), y - ( -vec.y ) ) +
		pow( z - ( -vec.z ), z - ( -vec.z ) )
	);
}

float Vector3D::Magnitude()
{
	float squared = this->Square();
	return sqrt( squared );
}

float Vector3D::ShowX() const noexcept
{
	return this->x;
}

float Vector3D::ShowY() const noexcept
{
	return this->y;
}

float Vector3D::ShowZ() const noexcept
{
	return this->z;
}

void Vector3D::Display() noexcept
{
	std::ostringstream oss;
	oss << "x: " << x;
	oss << "\ty: " << y;
	oss << "\tz: " << z;
	OutputDebugStringA( oss.str().c_str() );
}