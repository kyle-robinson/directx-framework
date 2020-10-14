#include "WindowContainer.h"

bool RenderWindow::Initialize( WindowContainer* pWindowContainer, HINSTANCE hInstance, const std::string& windowName, const std::string& windowClass, int width, int height )
{
	// register window class
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->windowTitle = windowName;
	this->windowTitle_Wide = StringConverter::StringToWide( windowName );
	this->windowClass = windowClass;
	this->windowClass_Wide = StringConverter::StringToWide( windowClass );

	hHandNormal = LoadCursor( this->hInstance, (LPCWSTR)IDR_ANICURSOR1 );
	hHandSelect = LoadCursor( this->hInstance, (LPCWSTR)IDR_ANICURSOR2 );

	this->RegisterWindowClass();

	int centerScreenX = GetSystemMetrics( SM_CXSCREEN ) / 2 - this->width / 2;
	int centerScreenY = GetSystemMetrics( SM_CYSCREEN ) / 2 - this->height / 2;

	RECT windowRect;
	windowRect.left = centerScreenX;
	windowRect.top = centerScreenY;
	windowRect.right = windowRect.left + this->width;
	windowRect.bottom = windowRect.top + this->height;
	AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );

	// create window
	this->hWnd = CreateWindow(
		this->windowClass_Wide.c_str(),
		this->windowTitle_Wide.c_str(),
		WS_OVERLAPPEDWINDOW,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		this->hInstance,
		pWindowContainer
	);

	if ( this->hWnd == NULL )
	{
		ErrorLogger::Log( GetLastError(), "ERROR::CreateWindow Failed for window: " + this->windowTitle );
		return false;
	}

	// show window
	ShowWindow( this->hWnd, SW_SHOW );
	SetForegroundWindow( this->hWnd );
	SetFocus( this->hWnd );

	return true;
}

LRESULT CALLBACK HandleMsgRedirect( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch ( uMsg )
	{
	case WM_CLOSE:
		DestroyWindow( hWnd );
		return 0;

	// handle all other messages
	default:
	{
		// get ptr to window class
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
		// forward messages to window class
		return pWindow->WindowProc( hWnd, uMsg, wParam, lParam );
	}
	}
}

LRESULT CALLBACK HandleMsgSetup( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch ( uMsg )
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>( lParam );
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>( pCreate->lpCreateParams );
		if ( pWindow == nullptr )
		{
			ErrorLogger::Log( "ERROR::Pointer to window container is null during WM_NCCREATE!" );
			exit( -1 );
		}
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pWindow ) );
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( HandleMsgRedirect ) );
		return pWindow->WindowProc( hWnd, uMsg, wParam, lParam );
	}

	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
}

bool RenderWindow::ProcessMessages() noexcept
{
	MSG msg;
	ZeroMemory( &msg, sizeof( MSG ) );
	while ( PeekMessage( &msg, this->hWnd, 0u, 0u, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// check if the window was closed
	if ( msg.message == WM_NULL )
	{
		if ( !IsWindow( this->hWnd ) )
		{
			this->hWnd = NULL;
			UnregisterClass( this->windowClass_Wide.c_str(), this->hInstance );
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const noexcept
{
	return this->hWnd;
}

void RenderWindow::RegisterWindowClass() noexcept
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof( wc );
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( hInstance, (LPCTSTR)IDI_TUTORIAL1 );
    wc.hCursor = hHandNormal;
    wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClass_Wide.c_str();
	wc.hIconSm = LoadIcon( wc.hInstance, (LPCTSTR)IDI_TUTORIAL1 );
	RegisterClassEx( &wc );
}

RenderWindow::~RenderWindow() noexcept
{
	if ( this->hWnd != NULL )
	{
		UnregisterClass( this->windowClass_Wide.c_str(), this->hInstance );
		DestroyWindow( this->hWnd );
	}
}