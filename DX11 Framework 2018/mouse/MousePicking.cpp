#include "MousePicking.h"
#include "../graphics/Graphics.h"
#include <DirectXCollision.h>

void MousePicking::Initialize( XMMATRIX viewMatrix, XMMATRIX projectionMatrix, int width, int height )
{
	this->viewMatrix = viewMatrix;
	this->projectionMatrix = projectionMatrix;
	this->width = width;
	this->height = height;
}

void MousePicking::TestIntersection( int mouseX, int mouseY )
{
	XMMATRIX invProjection = XMMatrixInverse( nullptr, viewMatrix );
	XMMATRIX invView = XMMatrixInverse( nullptr, projectionMatrix );

	// Move the mouse cursor coordinates into the -1 to +1 range.
	float pointX = ( 2.0f * static_cast<float>( mouseX ) ) / static_cast<float>( width ) - 1.0f;
	float pointY = 1.0f - ( 2.0f * static_cast<float>( mouseY ) ) / static_cast<float>( height );

	XMVECTOR eyePos, dummy;
	XMMatrixDecompose( &dummy, &dummy, &eyePos, invView );

	// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.

	XMVECTOR rayOri = XMVectorSet( pointX, pointY, 0.0f, 0.0f );
	rayOri = XMVector3Transform( rayOri, invProjection );
	rayOri = XMVector3Transform( rayOri, invView );

	XMVECTOR rayDir = rayOri - eyePos;
	rayDir = XMVector3Normalize( rayDir );

	this->rayDirection = rayDir;
	this->rayOrigin = rayOri;

	// Now perform the ray-sphere intersection test.
	/*bool intersect = RaySphereIntersect( rayOrigin, rayDir, 1.0f );

	if ( intersect == true )
	{
		// If it does intersect then set the intersection to "yes" in the text string that is displayed to the screen.
		exit( -1 );
	}
	else
	{
		// If not then set the intersection to "No".
		
	}*/

	return;
}

void MousePicking::UpdateMatrices( XMMATRIX viewMatrix, XMMATRIX projectionMatrix )
{
	this->viewMatrix = viewMatrix;
	this->projectionMatrix = projectionMatrix;
}

bool MousePicking::RaySphereIntersect( XMFLOAT3 objectPos )
{
	BoundingSphere bs;
	bs.Center = objectPos;
	bs.Radius = 1;

	float distance = 0.0f;
	if ( bs.Intersects( rayOrigin, rayDirection, distance ) )
		return true;

	// Calculate the a, b, and c coefficients.
	/*float a = ( XMVectorGetX( rayDirection ) * XMVectorGetX( rayDirection ) ) +
		( XMVectorGetY( rayDirection ) * XMVectorGetY( rayDirection ) ) +
		( XMVectorGetZ( rayDirection ) * XMVectorGetZ( rayDirection ) );

	float b = ( ( XMVectorGetX( rayDirection ) * XMVectorGetX( rayOrigin ) ) +
		( XMVectorGetY( rayDirection ) * XMVectorGetY( rayOrigin ) ) +
		( XMVectorGetZ( rayDirection ) * XMVectorGetZ( rayOrigin ) ) ) * 2.0f;

	float c = ( ( XMVectorGetX( rayOrigin ) * XMVectorGetX( rayOrigin ) ) +
		( XMVectorGetY( rayOrigin ) * XMVectorGetY( rayOrigin ) ) +
		( XMVectorGetZ( rayOrigin ) * XMVectorGetZ( rayOrigin ) ) ) - ( radius * radius );

	// Find the discriminant.
	float discriminant = ( b * b ) - ( 4 * a * c );

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
	if ( discriminant < 0.0f )
		return false;*/

	return false;
}