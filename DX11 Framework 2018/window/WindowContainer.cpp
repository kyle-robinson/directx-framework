#include "WindowContainer.h"
#include <memory>

WindowContainer::WindowContainer()
{
	static bool rawInputInitialized = false;
	if ( !rawInputInitialized )
	{
		RAWINPUTDEVICE rid;
		ZeroMemory( &rid, sizeof( RAWINPUTDEVICE ) );
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if ( RegisterRawInputDevices( &rid, 1, sizeof( rid ) ) == FALSE )
		{
			ErrorLogger::Log( GetLastError(), "Failed to register raw input devices!" );
			exit( -1 );
		}

		rawInputInitialized = true;
	}
}

LRESULT CALLBACK WindowContainer::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
		return 0;

    case WM_DESTROY:
		DestroyWindow( hWnd );
		PostQuitMessage( 0 );
		exit( -1 );
		return 0;
	// Keyboard Events
	case WM_KEYDOWN:
	{
		switch ( wParam )
		{
		case VK_ESCAPE:
			DestroyWindow( hWnd );
			PostQuitMessage( 0 );
			exit( -1 );
			return 0;
		}
		return 0;
	}
	// Mouse Events
	case WM_LBUTTONDOWN:
	{
		SetCursor( renderWindow.hHandSelect );
		return 0;
	}
	case WM_LBUTTONUP:
	{
		SetCursor( renderWindow.hHandNormal );
		return 0;
	}
	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
}