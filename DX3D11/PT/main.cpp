#include "pch.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")		// D3DCompileFromFile LinkError Solution

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
HWND g_mainWindow;

ID3D11Device* g_device;
ID3D11DeviceContext* g_devcon;
IDXGISwapChain* g_swapChain;
ID3D11RenderTargetView* g_renderTargetView;
ID3D11RasterizerState* g_rasterizerState;

// depth buffer
ID3D11Texture2D* g_depthStencilBuffer;
ID3D11DepthStencilView* g_depthStencilView;
ID3D11DepthStencilState* g_depthStencilState;

D3D11_VIEWPORT g_viewport;

ID3D11VertexShader* pVS;						// the pointer to vertex shader
ID3D11PixelShader* pPS;							// the pointer to pixel shader
ID3D11Buffer* pVBuffer;							// the pointer to vertex buffer
ID3D11InputLayout* pLayout;					// the pointer to the input layout

// a struct to define a single vertex
struct VERTEX { FLOAT X , Y , Z; D3D11_VIDEO_COLOR_RGBA Color; };

bool InitD3D ();
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

	g_mainWindow = mainWindow;
	ShowWindow ( g_mainWindow , SW_SHOWDEFAULT );
	UpdateWindow ( g_mainWindow );

	if ( !InitD3D () ) {
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

bool InitD3D () {
	// Creating [ device, device_context, swap_chain, render_target_view, viewport, rasterizer_state ]
	// Creating [ depth_stencil_buffer, depth_stencil_view, 

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;	// D3D_DRIVER_TYPE_WARP( Driver 호환성 문제가 발생하면 )
	const D3D_FEATURE_LEVEL featureLevels[ 2 ] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_9_3
	};

	ID3D11Device* device; 
	ID3D11DeviceContext* devcon;
	D3D_FEATURE_LEVEL featureLevel;

	if ( FAILED ( D3D11CreateDevice (
		nullptr ,
		driverType ,
		0 ,
		createDeviceFlags ,
		featureLevels ,
		ARRAYSIZE ( featureLevels ) ,
		D3D11_SDK_VERSION ,
		&device ,					// out
		&featureLevel ,		// out
		&devcon						// out
	) ) ) {
		std::cout << "D3D11CreateDevice() failed" << std::endl;
		return false;
	}

	// version이 맞지 않으면 종료, 더 낮은 version으로 바꾸는 경우도 있음
	if ( featureLevel != D3D_FEATURE_LEVEL_11_0 ) {
		std::cout << "D3D Feature Level 11 unsupported" << std::endl;
		return false;
	}

	// ComPtr::AS()는 내부적으로 QueryInterface()를 호출해서 해당 interface로의 casting이 가능한지 여부를 HRESULT로 반환한다.
	// 이런 방식으로 device와 context에 문제가 없음을 알고 싶다면, QueryInterface()를 직접 호출한다.
	if ( FAILED ( device->QueryInterface ( __uuidof( ID3D11Device ) , ( void** ) &g_device ) ) ) {
		std::cout << "device->QueryInterface() failed" << std::endl;
		return false;
	}
	if ( FAILED ( devcon->QueryInterface ( __uuidof( ID3D11DeviceContext ) , ( void** ) &g_devcon ) ) ) {
		std::cout << "devcon->QueryInterface() failed" << std::endl;
		return false;
	}
	g_device = device;
	g_devcon = devcon;
	device->Release ();
	devcon->Release ();

	// DirectX에서 지원하는 MSAA를 hardware가 지원하나?
	// swap chain과 depth buffer에서 MSAA 설정을 이용한다. 
	UINT numQualityLevels;
	g_device->CheckMultisampleQualityLevels ( DXGI_FORMAT_R8G8B8A8_UNORM , 4 , &numQualityLevels );
	if ( numQualityLevels <= 0 ) {
		std::cout << "MSAA not supported" << std::endl;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory ( &scd , sizeof ( scd ) );
	scd.BufferCount = 2;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = g_mainWindow;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	if ( numQualityLevels > 0 ) {
		scd.SampleDesc.Count = 4;
		scd.SampleDesc.Quality = numQualityLevels - 1;
	}
	else {
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
	}

	if ( FAILED ( D3D11CreateDeviceAndSwapChain (
		0,
		driverType ,
		0 ,
		createDeviceFlags ,
		featureLevels ,
		1 ,
		D3D11_SDK_VERSION ,
		&scd ,
		&g_swapChain ,
		&g_device ,
		&featureLevel ,
		&g_devcon ) ) ) {
		std::cout << "D3D11CreateDeviceAndSwapChain() failed." << std::endl;
		return false;
	}

	// Set the render target
	ID3D11Texture2D* pBackBuffer;
	g_swapChain->GetBuffer ( 0 , IID_PPV_ARGS(&pBackBuffer ));
	if ( pBackBuffer ) {
		g_device->CreateRenderTargetView ( pBackBuffer , NULL , &g_renderTargetView );
		pBackBuffer->Release ();
	}
	else {
		std::cout << "CreateRenderTargetView() failed." << std::endl;
		return false;
	}
	

	// Set the viewport
	ZeroMemory ( &g_viewport , sizeof ( D3D11_VIEWPORT ) );
	g_viewport.TopLeftX = 0;
	g_viewport.TopLeftY = 0;
	g_viewport.Width = float(SCREEN_WIDTH);
	g_viewport.Height = float(SCREEN_HEIGHT);
	// Depth Buffering을 사용하기 위한 options
	g_viewport.MinDepth = 0.0f;
	g_viewport.MaxDepth = 1.0f;
	g_devcon->RSSetViewports ( 1 , &g_viewport );	// Rasterization Stage( 3D coordinates(world) -> 2D coordinates(screen) )

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory ( &rastDesc , sizeof ( D3D11_RASTERIZER_DESC ) );
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	g_device->CreateRasterizerState ( &rastDesc , &g_rasterizerState );

	// Create Depth Buffer & Stencil Buffer
	// depth 값을 저장하는 buffer( memory )
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = SCREEN_WIDTH;
	depthStencilBufferDesc.Height = SCREEN_HEIGHT;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// Depth: unsigned normalized int 24bit, Stencil: unsigned int 8bit
	if ( numQualityLevels > 0 ) {
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
	}
	else {
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;		// texture memory를 어떻게 사용할 것인가?
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	if ( FAILED ( g_device->CreateTexture2D ( &depthStencilBufferDesc , 0 , &g_depthStencilBuffer ) ) ) {
		std::cout << "CreateTexture2D() failed" << std::endl;
	}
	// 이를 통해 생성한 DepthStencilView를 이용해서 DepthStencilBuffer를 사용한다.
	if ( FAILED ( g_device->CreateDepthStencilView ( g_depthStencilBuffer , 0 , &g_depthStencilView ) ) ) {
		std::cout << "CreateDepthStencilView() failed" << std::endl;
	}

	// DepthStencilView를 어떤 상태로 사용하나?
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory ( &depthStencilDesc , sizeof ( D3D11_DEPTH_STENCIL_DESC ) );
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	// depth 값이 더 작거나 같으면 화면에 그린다.
	if ( FAILED ( g_device->CreateDepthStencilState ( &depthStencilDesc , &g_depthStencilState ) ) ) {
		std::cout << "CreateDepthStencilState() failed" << std::endl;
	}

	InitPipeline ();
	InitGraphics ();

	//g_devcon->OMSetRenderTargets ( 1 , &g_renderTargetView , NULL );

	return true;
}

void RenderFrame () {
	// clear the back buffer to a deep blue
	float clearColor[ 4 ] = { 0.0f, 0.2f, 0.4f, 1.0f };
	g_devcon->ClearRenderTargetView ( g_renderTargetView , clearColor);

	// select which vertex buffer to display
	UINT stride = sizeof ( VERTEX );
	UINT offset = 0;
	g_devcon->IASetVertexBuffers ( 0 , 1 , &pVBuffer , &stride , &offset );
	g_devcon->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	g_devcon->Draw(3, 0);

	// switch the back buffer and the front buffer
	g_swapChain->Present ( 0 , 0 );
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
	g_swapChain->SetFullscreenState ( FALSE , NULL );

	g_device->Release ();
	g_devcon->Release ();
	g_swapChain->Release ();
	g_renderTargetView->Release ();
	g_rasterizerState->Release ();
	g_depthStencilBuffer->Release ();
	g_depthStencilView->Release ();
	g_depthStencilState->Release ();

	pLayout->Release ();
	pVS->Release ();
	pPS->Release ();
	pVBuffer->Release ();
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
	g_device->CreateVertexShader ( VS->GetBufferPointer () , VS->GetBufferSize () , NULL , &pVS );
	g_device->CreatePixelShader ( PS->GetBufferPointer () , PS->GetBufferSize () , NULL , &pPS );

	// set the shader objects
	g_devcon->VSSetShader ( pVS , 0 , 0 );
	g_devcon->PSSetShader ( pPS , 0 , 0 );

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	g_device->CreateInputLayout ( ied , 2 , VS->GetBufferPointer () , VS->GetBufferSize () , &pLayout );
	g_devcon->IASetInputLayout ( pLayout );
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

	g_device->CreateBuffer ( &bd , NULL , &pVBuffer );		// create the buffer

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	g_devcon->Map ( pVBuffer , NULL , D3D11_MAP_WRITE_DISCARD , NULL , &ms );
	memcpy ( ms.pData , OurVertices , sizeof ( OurVertices ) );
	g_devcon->Unmap ( pVBuffer , NULL );
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