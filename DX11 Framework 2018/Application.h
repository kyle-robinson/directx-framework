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
	XMFLOAT4 ambientLight = { 0.2f, 0.2f, 0.2f, 1.0f };
	XMFLOAT4 diffuseMaterial = { 0.8f, 0.5f, 0.5f, 1.0f };
	XMFLOAT4 diffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT4 specularLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	XMFLOAT3 lightDirection = { 0.0f, 0.0f, -1.0f };
	float specularPower = 10.0f;
private:
	SYSTEM_INFO siSysInfo;
	Timer timer;
};

#endif