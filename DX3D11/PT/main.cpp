#include "pch.h"

LRESULT CALLBACK WndProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam ) {
	switch ( msg ) {
	case WM_SIZE:
		// Reset and resize swapchain
		break;
	case WM_SYSCOMMAND:
		if ( ( wParam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
			return 0;
		break;
	case WM_MOUSEMOVE:
		// cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << endl;
		break;
	case WM_LBUTTONUP:
		// cout << "WM_LBUTTONUP Left mouse button" << endl;
		break;
	case WM_RBUTTONUP:
		// cout << "WM_RBUTTONUP Right mouse button" << endl;
		break;
	case WM_KEYDOWN:
		// cout << "WM_KEYDOWN " << (int)wParam << endl;
		break;
	case WM_DESTROY:
		::PostQuitMessage ( 0 );
		return 0;
	}

	return DefWindowProc ( hwnd , msg , wParam , lParam );
}

int main ()
{
	// Create Window Instance
	WNDCLASSEX wc = {
		sizeof ( WNDCLASSEX ),			// cbSize: WNDCLASSEX structure의 크기
		CS_CLASSDC,									// style: Window Class Style
		WndProc,										// lpfnWndProc: Window Processor Function Pointer
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		L"PT",
		NULL
	};

	// Register Window Instance( Window 객체 등록 )
	if ( !RegisterClassEx ( &wc ) ) {
		std::cout << "RegisterClassEx() failed." << std::endl;
		return -1;
	}

	int screenWidth = 1280;
	int screenHeight = 960;
	RECT wr = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect ( &wr , WS_OVERLAPPEDWINDOW , false );

	HWND mainWindow = CreateWindow(wc.lpszClassName, L"PT WND", WS_OVERLAPPEDWINDOW,
																100, 
																100,
																wr.right - wr.left,
																wr.bottom - wr.top,
																NULL , NULL , wc.hInstance , NULL );
	if ( !mainWindow ) {
		std::cout << "CreateWindow() failed." << std::endl;
		return -1;
	}

	ShowWindow ( mainWindow , SW_SHOWDEFAULT );
	UpdateWindow ( mainWindow );

	// main message loop
	MSG msg = { 0 };
	while ( WM_QUIT != msg.message ) {
		if ( PeekMessage ( &msg , NULL , 0 , 0 , PM_REMOVE ) ) {
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
		}
		else {
			std::cout << "Rendering..." << std::endl;
		}
	}

	return 0;
}