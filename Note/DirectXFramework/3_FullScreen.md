- [Introduce](#introduce)
- [Setting Up the Screen Size](#setting-up-the-screen-size)
- [Changing to Fullscreen Mode](#changing-to-fullscreen-mode)
	- [1. Modify the window to have no background](#1-modify-the-window-to-have-no-background)
	- [2. Set the back buffer to a specific size](#2-set-the-back-buffer-to-a-specific-size)
	- [3. Set DirectX to automatically switch when Alt-Enter is used](#3-set-directx-to-automatically-switch-when-alt-enter-is-used)
	- [4. Modify the CleanD3D() function to turn off fullscreen when closing](#4-modify-the-cleand3d-function-to-turn-off-fullscreen-when-closing)
- [Final code](#final-code)

[Going Fullscreen - DirectXTutorial](http://www.directxtutorial.com/Lesson.aspx?lessonid=11-4-4)   
 
# Introduce
screen resolution을 gloabl하게 설정하는 방법과 그 이유에 대해 살펴보고, window과 fullscreen mode를 넘나드는 기법에 대해 알아본다.   

# Setting Up the Screen Size
DirectX를 사용하여 game program을 만들다보면 screen size를 알아야하는 많은 function과 struct를 접하게 된다. 나중에 resolution을 변경할 때, 특히 run-time 중 resolution을 변경할 때 번거로울 수 있다.   
따라서 program 전체에서 screen size를 표준화하는 간단한 방법을 알아본다.   

```cpp
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
```
먼저 screen width와 height를 나타내는 two directives를 추가한다.   
```cpp
RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
AdjustWindowRect ( &wr , WS_OVERLAPPEDWINDOW , FALSE );

HWND mainWindow = CreateWindowEx ( 
	NULL ,
	wc.lpszClassName ,            // name of the window class
	L"window1 title" ,            // title of the window
	WS_OVERLAPPEDWINDOW ,         // window style
	0 ,                           // x-position of the window
	0 ,                           // y-position of the window
	wr.right - wr.left ,
	wr.bottom - wr.top ,
	NULL ,                        // we have no parent window
	NULL ,                        // we aren't using menus
	wc.hInstance ,                // apllication handle
	NULL // used with multiple windows
);
```
```cpp
viewPort.Width = SCREEN_WIDTH;
viewPort.Height = SCREEN_HEIGHT;
```
다음에는 client와 viewport 크기를 `SCREEN_WIDTH, SCREEN_HEIGHT`로 통일한다.   
이전에 말했던, [viewport와 back buffer의 크기가 다른 경우](2_RenderingFrames.md/#3-back-buffer과-viewport-크기의-상관관계) 발생하는 다양한 문제점을 방지할 수 있다.   

# Changing to Fullscreen Mode
많은 게임들은 fullscreen으로 플레이하지만, window mode도 지원한다. 따라서 기본값은 fullscreen mode지만, user의 희망에 따라 window mode로 바꿀 수 있는 option을 제공한다. 또한 이러한 단축키로 "Alt + Enter"를 지원한다.   

## 1. Modify the window to have no background
window background를 제거하기 위해서 `WINDOWCLASSEX`의 멤버 변수에서 background color를 세팅하는 값을 comment 처리한다.   
```cpp
// wc.hbrBackground = ( HBRUSH ) COLOR_WINDOW;
```

## 2. Set the back buffer to a specific size
`scd` struct를 약간 수정해서 DirectX에게 screen resolution을 알린다.   
```cpp
bool InitD3D( HWND hWnd ) {
  DXGI_SWAP_CHAIN_DESC scd;
  // ...
  scd.BufferDesc.Width = SCREEN_WIDTH;    // set the back buffer width
  scd.BufferDesc.Height = SCREEN_HEIGHT;  // set the back buffer height
}
```

## 3. Set DirectX to automatically switch when Alt-Enter is used
`scd` struct에 flag를 추가한다.   
```cpp
scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // allow full-screen switching
```
`DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH`는 swap chain이 fullscreen-window mode간의 전환을 허용하는 flag이다.   
**Direct3D가 "Alt+Enter" 키 입력을 감지하여 fullscreen과 window mode간의 전환이 자동으로 이뤄지도록 처리**한다.   

## 4. Modify the CleanD3D() function to turn off fullscreen when closing
Direct3D는 fullscreen mode에서 프로그램을 닫을 수 없다. 이는 background에서 발생하는 특정 threading 문제다.   
따라서 올바르게 종료하려면, window mode임을 확인해야 한다. 이를 위해 `SetFullscreenState()`를 사용한다.   
```cpp
void CleanD3D() {
  swapChain->SetFullscreenState(FALSE, NULL);   // switch to windowed mode

  // close and release all existing COM objects
}
```
`SetFullscreenState`의 첫 인자는 전환할 상태이다. `FALSE`는 window mode를 나타내고, `TRUE`는 fullscreen mode를 나타낸다.   
두 번째 인자는 사용할 video adapter를 선택할 수 있는 기능이다. 이는 multiple monitors를 사용할 때 유용하다. 하지만 대부분 모든 게임에서 `NULL`로 세팅한다.   

즉, **이 함수를 통해 window mode로 바꾸고 COM objects를 release한다**.   

# Final code
```cpp
#include "pch.h"

#pragma comment (lib, "d3d11.lib")

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960

IDXGISwapChain* swapChain;					// the pointer to the swap chain interface
ID3D11Device* dev;									// the pointer to our Direct3D device interface
ID3D11DeviceContext* devcon;				// the pointer to our Direct3D device context
ID3D11RenderTargetView* backBuffer;	// the pointer to our back buffer

bool InitD3D ( HWND hWnd );
void RenderFrame ();
void CleanD3D ();
LRESULT CALLBACK WndProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam );

int main ()
{
	WNDCLASSEX wc;
	ZeroMemory ( &wc , sizeof ( WNDCLASSEX ) );

	wc.cbSize = sizeof ( WNDCLASSEX );
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor ( NULL , IDC_ARROW );
	//wc.hbrBackground = ( HBRUSH ) COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	if ( !RegisterClassEx ( &wc ) ) {
		std::cout << "RegisterClassEx() failed." << std::endl;
		return -1;
	}

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect ( &wr , WS_OVERLAPPEDWINDOW , FALSE );

	HWND mainWindow = CreateWindowEx ( 
		NULL ,
		wc.lpszClassName ,     // name of the window class
		L"window1 title" ,     // title of the window
		WS_OVERLAPPEDWINDOW ,  // window style
		0 ,                    // x-position of the window
		0 ,                    // y-position of the window
		wr.right - wr.left ,   // width of the window
		wr.bottom - wr.top ,   // height of the window
		NULL ,                 // we have no parent window
		NULL ,                 // we aren't using menus
		wc.hInstance ,         // apllication handle
		NULL									 // used with multiple windows
	);

	if ( !mainWindow ) {
		std::cout << "CreateWindow() failed." << std::endl;
		return -1;
	}

	ShowWindow ( mainWindow , SW_SHOWDEFAULT );
	UpdateWindow ( mainWindow );

	if ( !InitD3D ( mainWindow ) ) {
		std::cout << "InitD3D() failed." << std::endl;
		return -1;
	}

	MSG msg = { 0 };
	// check to see if it is time to quit
	while ( WM_QUIT != msg.message ) {
		// check to see if any messages are waiting in the queue
		if ( PeekMessage ( &msg , NULL , 0 , 0 , PM_REMOVE ) ) {
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
		}
		else {
			RenderFrame ();
		}
	}

	CleanD3D ();

	return 0;
}

bool InitD3D ( HWND hWnd ) {

	// Init Direct3D
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory ( &scd , sizeof ( scd ) );
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if ( FAILED ( D3D11CreateDeviceAndSwapChain ( NULL ,
		D3D_DRIVER_TYPE_HARDWARE ,
		NULL ,
		NULL ,
		NULL ,
		NULL ,
		D3D11_SDK_VERSION ,
		&scd ,
		&swapChain ,
		&dev ,
		NULL ,
		&devcon ) ) ) {
		std::cout << "D3D11CreateDeviceAndSwapChain() failed." << std::endl;
		return false;
	}

	// Set the render target
	ID3D11Texture2D* pBackBuffer;
	swapChain->GetBuffer ( 0 , __uuidof( ID3D11Texture2D ) , ( LPVOID* ) &pBackBuffer );
	if ( pBackBuffer ) {
		dev->CreateRenderTargetView ( pBackBuffer , NULL , &backBuffer );
		pBackBuffer->Release ();
	}
	else {
		std::cout << "CreateRenderTargetView() failed." << std::endl;
		return false;
	}
	devcon->OMSetRenderTargets ( 1 , &backBuffer , NULL );

	// Set the viewport
	D3D11_VIEWPORT viewPort;
	ZeroMemory ( &viewPort , sizeof ( D3D11_VIEWPORT ) );
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = 1280;
	viewPort.Height = 960;
	devcon->RSSetViewports ( 1 , &viewPort );

	return true;
}

void RenderFrame () {
	// clear the back buffer to a deep blue
	float clearColor[ 4 ] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView ( backBuffer , clearColor);

	// switch the back buffer and the front buffer
	swapChain->Present ( 0 , 0 );

	std::cout << "Rendering..." << std::endl;
}

void CleanD3D () {
	swapChain->SetFullscreenState ( FALSE , NULL );

	swapChain->Release ();
	backBuffer->Release ();
	dev->Release ();
	devcon->Release ();
}

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
```