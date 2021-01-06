#pragma once
#ifndef BILLBOARDING_H
#define BILLBOARDING_H

#include "..\\graphics\GameObject3D.h"
#include "..\\graphics\\Camera3D.h"

class Billboarding
{
public:
	static float BillboardModel( std::shared_ptr<Camera3D>& camera, GameObject3D& object )
	{
		double angle = atan2( object.GetPositionFloat3().x - camera->GetPositionFloat3().x,
			object.GetPositionFloat3().z - camera->GetPositionFloat3().z ) * ( 180.0 / XM_PI );
		return static_cast<float>( angle ) * 0.0174532925f;
	}
};

#endif