#include "pch.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")		// D3DCompileFromFile LinkError Solution

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960

IDXGISwapChain* swapChain;					// the pointer to the swap chain interface
ID3D11Device* dev;									// the pointer to our Direct3D device interface
ID3D11DeviceContext* devcon;				// the pointer to our Direct3D device context
ID3D11RenderTargetView* backBuffer;	// the pointer to our back buffer
ID3D11VertexShader* pVS;						// the pointer to vertex shader
ID3D11PixelShader* pPS;							// the pointer to pixel shader
ID3D11Buffer* pVBuffer;							// the pointer to vertex buffer
ID3D11InputLayout* pLayout;					// the pointer to the input layout

// a struct to define a single vertex
struct VERTEX { FLOAT X , Y , Z; D3D11_VIDEO_COLOR_RGBA Color; };

bool InitD3D ( HWND hWnd );
void RenderFrame ();
void InitPipeline ();
void CleanD3D ();
void InitGraphics ();
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

	InitPipeline ();
	InitGraphics ();

	return true;
}

void RenderFrame () {
	// clear the back buffer to a deep blue
	float clearColor[ 4 ] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView ( backBuffer , clearColor);

	// select which vertex buffer to display
	UINT stride = sizeof ( VERTEX );
	UINT offset = 0;
	devcon->IASetVertexBuffers ( 0 , 1 , &pVBuffer , &stride , &offset );
	devcon->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	devcon->Draw(3, 0);

	// switch the back buffer and the front buffer
	swapChain->Present ( 0 , 0 );
}

void CheckResult ( HRESULT hr , ID3DBlob* errorBlob ) {
	if ( FAILED ( hr ) ) {
		// not exist file
		if ( ( hr & D3D11_ERROR_FILE_NOT_FOUND ) != 0 ) {
			std::cout << "File not found" << std::endl;
		}

		// output error message if exist error message
		if ( errorBlob ) {
			std::cout << "Shader compile error\n" << ( char* ) errorBlob->GetBufferPointer () << std::endl;
		}
	}
}

void CleanD3D () {
	swapChain->SetFullscreenState ( FALSE , NULL );

	pLayout->Release ();
	pVS->Release ();
	pPS->Release ();
	pVBuffer->Release ();
	swapChain->Release ();
	backBuffer->Release ();
	dev->Release ();
	devcon->Release ();
}

void InitPipeline () {
	// load and compile the two shaders
	ID3DBlob* VS{} , * PS{};
	ID3DBlob* VSErrorBlob, * PSErrorBlob;
	HRESULT vsHr = D3DCompileFromFile ( L"shaders.shader" , 0 , 0 , "VShader" , "vs_4_0" , 0 , 0 , &VS , &VSErrorBlob );
	HRESULT psHr = D3DCompileFromFile ( L"shaders.shader" , 0 , 0 , "PShader" , "ps_4_0" , 0 , 0 , &PS , &PSErrorBlob );

	// check error
	CheckResult ( vsHr , VSErrorBlob );
	CheckResult ( psHr , PSErrorBlob );

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader ( VS->GetBufferPointer () , VS->GetBufferSize () , NULL , &pVS );
	dev->CreatePixelShader ( PS->GetBufferPointer () , PS->GetBufferSize () , NULL , &pPS );

	// set the shader objects
	devcon->VSSetShader ( pVS , 0 , 0 );
	devcon->PSSetShader ( pPS , 0 , 0 );

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	dev->CreateInputLayout ( ied , 2 , VS->GetBufferPointer () , VS->GetBufferSize () , &pLayout );
	devcon->IASetInputLayout ( pLayout );
}

void InitGraphics () {
	// create a triangle
	VERTEX OurVertices[] = {
		{ 0.0f, 0.5f, 0.0f, D3D11_VIDEO_COLOR_RGBA ( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ 0.45f, -0.5f, 0.0f, D3D11_VIDEO_COLOR_RGBA ( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ -0.45f, -0.5f, 0.0f, D3D11_VIDEO_COLOR_RGBA ( 0.0f, 0.0f, 1.0f, 1.0f ) }
	};
	

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory ( &bd , sizeof ( bd ) );

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof ( VERTEX ) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dev->CreateBuffer ( &bd , NULL , &pVBuffer );		// create the buffer

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map ( pVBuffer , NULL , D3D11_MAP_WRITE_DISCARD , NULL , &ms );
	memcpy ( ms.pData , OurVertices , sizeof ( OurVertices ) );
	devcon->Unmap ( pVBuffer , NULL );
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