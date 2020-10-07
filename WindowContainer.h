#pragma once
#include "RenderWindow.h"
#include "Keyboard.h"
#include "Graphics.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	RenderWindow renderWindow;
	Keyboard keyboard;
	Graphics gfx;
};