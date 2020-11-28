#pragma once
#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "..\\graphics\GameObject3D.h"
#include "..\\graphics\\Camera3D.h"
#include "..\\graphics\\Light.h"

class GameObject3D;
class Camera3D;
class Light;

class Collisions
{
public:
	static bool CheckCollision3D( GameObject3D& object1, GameObject3D& object2, float radius );
	static bool CheckCollision3D( std::shared_ptr<Camera3D>& object1, Light& object2, float radius );
	static bool CheckCollision3D( std::shared_ptr<Camera3D>& object1, GameObject3D& object2, float radius, float yOffset = 0.0f );
};

#endif