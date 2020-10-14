#include "Application.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    UNREFERENCED_PARAMETER( nCmdShow );

    Application theApp;
	if ( theApp.Initialize( hInstance, "DX11 Framework", "TutorialWindowClass", 800, 600 ) )
	{
        while ( theApp.ProcessMessages() == true )
        {
            theApp.Update();
            theApp.Render();
        }
	}

    return 0;
}