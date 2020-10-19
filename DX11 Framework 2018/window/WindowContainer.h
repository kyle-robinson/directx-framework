#include "RenderWindow.h"
#include "../graphics/Graphics.h"
#include "../keyboard/Keyboard.h"
#include "../mouse/Mouse.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	RenderWindow renderWindow;
	Keyboard keyboard;
	Graphics gfx;
	Mouse mouse;
};