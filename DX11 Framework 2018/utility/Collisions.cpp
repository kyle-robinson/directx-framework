#include "Collisions.h"

bool Collisions::CheckCollision3D( GameObject3D& object1, GameObject3D& object2, float radius )
{
	if ( ( object1.GetPositionFloat3().x - object2.GetPositionFloat3().x ) *
         ( object1.GetPositionFloat3().x - object2.GetPositionFloat3().x ) +
         ( object1.GetPositionFloat3().y - object2.GetPositionFloat3().y ) *
         ( object1.GetPositionFloat3().y - object2.GetPositionFloat3().y ) +
         ( object1.GetPositionFloat3().z - object2.GetPositionFloat3().z ) *
         ( object1.GetPositionFloat3().z - object2.GetPositionFloat3().z ) <= radius * radius )
	     return true;
    else
         return false;
}

bool Collisions::CheckCollision3D( std::shared_ptr<Camera3D>& object1, Light& object2, float radius )
{
    if ( ( object1->GetPositionFloat3().x - object2.GetPositionFloat3().x ) *
         ( object1->GetPositionFloat3().x - object2.GetPositionFloat3().x ) +
         ( object1->GetPositionFloat3().y - object2.GetPositionFloat3().y ) *
         ( object1->GetPositionFloat3().y - object2.GetPositionFloat3().y ) +
         ( object1->GetPositionFloat3().z - object2.GetPositionFloat3().z ) *
         ( object1->GetPositionFloat3().z - object2.GetPositionFloat3().z ) <= radius * radius )
	     return true;
    else
         return false;
}

bool Collisions::CheckCollision3D( std::shared_ptr<Camera3D>& object1, GameObject3D& object2, float radius, float yOffset )
{
    XMFLOAT3 lookAtPos = object2.GetPositionFloat3();
    lookAtPos.y += yOffset;
    object1->SetLookAtPos( XMFLOAT3( lookAtPos.x, lookAtPos.y, lookAtPos.z ) );

    if ( ( object1->GetPositionFloat3().x - object2.GetPositionFloat3().x ) *
         ( object1->GetPositionFloat3().x - object2.GetPositionFloat3().x ) +
         ( object1->GetPositionFloat3().y - object2.GetPositionFloat3().y ) *
         ( object1->GetPositionFloat3().y - object2.GetPositionFloat3().y ) +
         ( object1->GetPositionFloat3().z - object2.GetPositionFloat3().z ) *
         ( object1->GetPositionFloat3().z - object2.GetPositionFloat3().z ) <= radius * radius )
    {
        return true;
    }
    else
        return false;
}