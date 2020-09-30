#include "Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Application theApp;
	if (FAILED(theApp.Initialise(hInstance, nCmdShow)))
	{
		return -1;
	}

    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			theApp.Update();
            theApp.Draw();
        }
    }

    return (int) msg.wParam;
}