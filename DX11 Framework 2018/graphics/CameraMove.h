#pragma once
#ifndef CAMERAMOVE_H
#define CAMERAMOVE_H

#include "Camera3D.h"

class CameraMove : public Camera3D
{
public:
	/// CAMERA MOVEMENT
	static void MoveForward( std::shared_ptr<Camera3D>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetForwardVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveBackward( std::shared_ptr<Camera3D>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetBackwardVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveLeft( std::shared_ptr<Camera3D>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetLeftVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveRight( std::shared_ptr<Camera3D>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( camera->GetRightVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveUp( std::shared_ptr<Camera3D>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( 0.0f, camera->GetCameraSpeed() * dt, 0.0f );
	}

	static void MoveDown( std::shared_ptr<Camera3D>& camera, float dt ) noexcept
	{
		camera->AdjustPosition( 0.0f, -camera->GetCameraSpeed() * dt, 0.0f );
	}

	/// MODEL MOVEMENT
	static void MoveForward( std::shared_ptr<Camera3D>& camera, RenderableGameObject& object, float dt ) noexcept
	{
		object.AdjustPosition( object.GetBackwardVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveBackward( std::shared_ptr<Camera3D>& camera, RenderableGameObject& object, float dt ) noexcept
	{
		object.AdjustPosition( object.GetForwardVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveLeft( std::shared_ptr<Camera3D>& camera, RenderableGameObject& object, float dt ) noexcept
	{
		object.AdjustPosition( object.GetRightVector() * camera->GetCameraSpeed() * dt );
	}

	static void MoveRight( std::shared_ptr<Camera3D>& camera, RenderableGameObject& object, float dt ) noexcept
	{
		object.AdjustPosition( object.GetLeftVector() * camera->GetCameraSpeed() * dt );
	}
};

#endif