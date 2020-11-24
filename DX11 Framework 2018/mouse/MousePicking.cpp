#include "MousePicking.h"
#include "../utility/Structs.h"
#include "../graphics/GameObject3D.h"
#include <DirectXCollision.h>

void MousePicking::Initialize( XMMATRIX viewMatrix, XMMATRIX projectionMatrix, int width, int height )
{
	this->viewMatrix = viewMatrix;
	this->projectionMatrix = projectionMatrix;
	this->width = width;
	this->height = height;
}

void MousePicking::UpdateMatrices( XMMATRIX viewMatrix, XMMATRIX projectionMatrix )
{
	this->viewMatrix = viewMatrix;
	this->projectionMatrix = projectionMatrix;
}

bool MousePicking::TestIntersection( int mouseX, int mouseY, GameObject3D object )
{
	XMMATRIX invProj = XMMatrixInverse( nullptr, projectionMatrix );
	XMMATRIX invView = XMMatrixInverse( nullptr, viewMatrix );

	// move the mouse cursor coordinates into the -1 to +1 range
	static float widthToUse;
	if ( viewportParams.useSplit && viewportParams.controlLeftSide )
		widthToUse = static_cast<float>( width ) * 0.5f;
	else
		widthToUse = static_cast<float>( width );
	float pointX = ( 2.0f * static_cast<float>( mouseX ) ) / widthToUse - 1.0f;
	float pointY = 1.0f - ( 2.0f * static_cast<float>( mouseY ) ) / static_cast<float>( height );

	XMVECTOR eyePos, dummy;
	XMMatrixDecompose( &dummy, &dummy, &eyePos, invView );

	// transform the mouse position into world space
	XMVECTOR rayOri = XMVectorSet( pointX, pointY, 0.0f, 0.0f );
	rayOri = XMVector3Transform( rayOri, invProj );
	rayOri = XMVector3Transform( rayOri, invView );

	XMVECTOR rayDir = rayOri - eyePos;
	rayDir = XMVector3Normalize( rayDir );

	// generate bounding sphere
	BoundingSphere bs;
	bs.Center = object.GetPositionFloat3();
		
	float maxRadius = 0.0f;
	if ( fabs( object.GetScaleFloat3().x > maxRadius ) )
		maxRadius = fabs( object.GetScaleFloat3().x );
	if ( fabs( object.GetScaleFloat3().y > maxRadius ) )
		maxRadius = fabs( object.GetScaleFloat3().y );
	if ( fabs( object.GetScaleFloat3().z > maxRadius ) )
		maxRadius = fabs( object.GetScaleFloat3().z );
	bs.Radius = maxRadius * 0.5f;

	// check for intersection between sphere and ray
	float distance = 1000.0f;
	if ( bs.Intersects( rayOri, rayDir, distance ) )
		return true;

	return false;
}