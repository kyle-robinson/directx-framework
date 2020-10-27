#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include "window/WindowContainer.h"
#include "utility/Timer.h"

class Application : public WindowContainer
{
public:
	bool Initialize(
		HINSTANCE hInstance,
		const std::string& windowTitle,
		const std::string& windowClass,
		int width,
		int height
	);
	bool ProcessMessages() noexcept;
	void Update();
	void Render();
private:
	float multiplier = 1.0f;
	float waterSpeed = 0.8f;
	float waterAmount = 0.01f;
	float waterHeight = 0.5f;
private:
	Timer timer;
};

#endif