#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include "window/WindowContainer.h"
#include "mouse/MousePicking.h"
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
	Timer timer;
	MousePicking mousePick;
};

#endif