- [Introduce](#introduce)
- [Code](#code)
  - [AppBase.h](#appbaseh)
  - [AppBase.cpp](#appbasecpp)

# Introduce
모든 App이 공통으로 사용하는 기능과 변수를 관리하는 최상위 부모 클래스   

# Code
## AppBase.h
```cpp
#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <wrl.h> // ComPtr

namespace pt {

using Microsoft::WRL::ComPtr;
using std::vector;
using std::wstring;

class AppBase {
public:
	AppBase ();
	virtual ~AppBase ();

	float GetAspectRatio () const;
	int Run ();

	virtual bool Initialize ();
	virtual void UpdateGUI () = 0;
	virtual void Update ( float dt ) = 0;
	virtual void Render () = 0;

	virtual LRESULT CALLBACK MsgProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam );

protected:
	bool InitMainWindow ();
	bool InitDirect3D ();
	bool InitGUI ();
	void CreateVertexShaderAndInputLayout ( const wstring& filename , 
		const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements ,
		ComPtr<ID3D11VertexShader>& vertexShader , ComPtr<ID3D11InputLayout>& inputLayout);
	void CreatePixelShader ( const wstring& filename , ComPtr<ID3D11PixelShader>& pixelShader );
	void CreateIndexBuffer ( const vector<uint16_t>& indices , ComPtr<ID3D11Buffer>& m_indexBuffer );

	template <typename T_VERTEX>
	void CreateVertexBuffer ( const vector<T_VERTEX>& vertices , ComPtr<ID3D11Buffer>& vertexBuffer ) {
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory ( &bufferDesc , sizeof ( bufferDesc ) );
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;	// 초기화 후 변경하지 않음
		bufferDesc.ByteWidth = UINT ( sizeof ( T_VERTEX ) * vertices.size () );
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof ( T_VERTEX );

		D3D11_SUBRESOURCE_DATA vertexBufferData{ 0 }; // ZeroMemory();
		vertexBufferData.pSysMem = vertices.data ();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		const HRESULT hr = m_device->CreateBuffer ( &bufferDesc , &vertexBufferData , vertexBuffer.GetAddressOf () );
		if ( FAILED ( hr ) ) {
			std::cout << "CreateVertexBuffer() - CreateBuffer() failed" << std::hex << hr << std::endl;
		}
	}

	template <typename T_CONSTANT>
	void CreateConstantBuffer ( const T_CONSTANT& constantBufferData , ComPtr<ID3D11Buffer>& constantBuffer ) {
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof ( constantBufferData );
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &constantBufferData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		const HRESULT hr = m_device->CreateBuffer ( &cbDesc , &initData , constantBuffer.GetAddressOf () );
		if ( FAILED ( hr ) ) {
			std::cout << "CreateConstantBuffer() - CreateBuffer() failed" << std::hex << hr << std::endl;
		}
	}

	template <typename T_DATA>
	void UpdateBuffer ( const T_DATA& bufferData , ComPtr<ID3D11Buffer>& buffer ) {
		D3D11_MAPPED_SUBRESOURCE ms;
		m_devcon->Map ( buffer.Get () , NULL , D3D11_MAP_WRITE_DISCARD , NULL , &ms );
		memcpy ( ms.pData , &bufferData , sizeof ( bufferData ) );
		m_devcon->Unmap ( buffer.Get () , NULL );
	}

public:
	// windows
	int m_screenWidth;
	int m_screenHeight;
	HWND m_mainWindow;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_devcon;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;

	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	D3D11_VIEWPORT m_viewport;
};

}	// namespace pt
```

## AppBase.cpp
```cpp
#include "AppBase.h"

// imgui_impl_win32.cpp에 정의된 메시지 처리 함수에 대한 전방 선언
// 해당 코드가 없으면 함수를 사용할 수 없다.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler ( 
	HWND hWnd , UINT msg , WPARAM wParam ,LPARAM lParam );

namespace pt {

	using namespace std;
	AppBase* g_appBase = nullptr;

	LRESULT CALLBACK WndProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam ) {
		return g_appBase->MsgProc (hwnd, msg, wParam, lParam);
	}

	AppBase::AppBase () 
		: m_screenWidth(1280), m_screenHeight(960), m_mainWindow(0),
			m_viewport(D3D11_VIEWPORT()) {
		g_appBase = this;
	}
	AppBase::~AppBase () {
		g_appBase = nullptr;

		// ComPtr은 smart pointer처럼 자동으로 release 작업을 수행한다.
	}

	float AppBase::GetAspectRatio () const { return float ( m_screenWidth ) / m_screenHeight; }
	int AppBase::Run () {
		MSG msg = { 0 };
		while ( WM_QUIT != msg.message ) {
			if ( PeekMessage ( &msg , NULL , 0 , 0 , PM_REMOVE ) ) {
				TranslateMessage ( &msg );
				DispatchMessage ( &msg );
			}
			else {
				// GUI frame 시작
				ImGui_ImplDX11_NewFrame ();
				ImGui_ImplWin32_NewFrame ();
				ImGui::NewFrame (); // Rendering 대상 기록 시작
				ImGui::Begin ("Scene Control");

				ImGui::Text ( "Average %.3f ms/frame (%.1f FPS)" , 1000.0f / ImGui::GetIO ().Framerate ,
					ImGui::GetIO ().Framerate );
				UpdateGUI ();

				ImGui::End ();
				ImGui::Render (); // Rendering 대상 기록 끝

				Update ( ImGui::GetIO ().DeltaTime ); // 매 프레임에 어떻게 움직이나?
				Render (); // Rendering
				ImGui_ImplDX11_RenderDrawData ( ImGui::GetDrawData () ); // GUI Rendering

				// switch the back buffer and the front buffer
				m_swapChain->Present (1, 0);
			}
		}

		return 0;
	}

	bool AppBase::Initialize () {
		if ( !InitMainWindow () ) {
			return false;
		}

		if ( !InitDirect3D () ) {
			return false;
		}

		if ( !InitGUI () ) {
			return false;
		}

		return true;
	}

	// windows
	LRESULT CALLBACK AppBase::MsgProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam ) {
		// ImGUI의 사용자 이벤트를 처리
		if ( ImGui_ImplWin32_WndProcHandler ( hwnd , msg , wParam , lParam ) ) {
			return true;
		}

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

	bool AppBase::InitMainWindow () {
		WNDCLASSEX wc;
		ZeroMemory ( &wc , sizeof ( WNDCLASSEX ) );
		wc.cbSize = sizeof ( WNDCLASSEX );
		wc.style = CS_CLASSDC;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandle ( NULL );
		wc.lpszClassName = L"WindowClass1";
		if ( !RegisterClassEx ( &wc ) ) {
			cout << "RegisterClassEx() failed" << endl;
			return false;
		}

		RECT wr = { 0, 0, m_screenWidth, m_screenHeight };
		AdjustWindowRect ( &wr , WS_OVERLAPPEDWINDOW , FALSE );
		m_mainWindow = CreateWindowEx ( 
			NULL ,
			wc.lpszClassName , L"window1 title" , WS_OVERLAPPEDWINDOW ,
			0 , 0 , wr.right - wr.left , wr.bottom - wr.top ,
			NULL , NULL ,
			wc.hInstance ,
			NULL );

		if ( !m_mainWindow ) {
			cout << "CreateWindowEx() failed" << endl;
			return false;
		}

		ShowWindow ( m_mainWindow, SW_SHOWDEFAULT );
		UpdateWindow ( m_mainWindow );

		return true;
	}

	bool AppBase::InitDirect3D () {
		// Creating [ device, device_context, swap_chain, render_target_view, viewport, rasterizer_state ]
		// Creating [ depth_stencil_buffer, depth_stencil_view ]

		// D3D_DRIVER_TYPE_WARP( Driver 호환성 문제가 발생하면 )
		const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		const D3D_FEATURE_LEVEL featureLevels[ 2 ] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_9_3
		};

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> devcon;
		D3D_FEATURE_LEVEL featureLevel;

		if ( FAILED ( D3D11CreateDevice (
			nullptr , driverType , 0 , createDeviceFlags ,
			featureLevels , ARRAYSIZE ( featureLevels ) ,
			D3D11_SDK_VERSION ,
			&device ,
			&featureLevel ,
			&devcon
		) ) ) {
			cout << "D3D11CreateDevice() failed" << endl;
			return false;
		}

		// version이 맞지 않으면 종료, 더 낮은 version으로 바꾸는 경우도 있음
		if ( featureLevel != D3D_FEATURE_LEVEL_11_0 ) {
			std::cout << "D3D Feature Level 11 unsupported" << std::endl;
			return false;
		}

		// DirectX에서 지원하는 MSAA를 hardware가 지원하나?
		// swap chain과 depth buffer에서 MSAA 설정을 이용한다. 
		UINT numQualityLevels;
		device->CheckMultisampleQualityLevels ( DXGI_FORMAT_R8G8B8A8_UNORM , 4 , &numQualityLevels );
		if ( numQualityLevels <= 0 ) {
			cout << "MSAA not supported" << endl;
		}

		if ( FAILED ( device.As ( &m_device ) ) ) {
			cout << "device.As() failed" << endl;
			return false;
		}
		if ( FAILED ( devcon.As ( &m_devcon ) ) ) {
			cout << "devcon.As() failed" << endl;
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory ( &scd , sizeof ( scd ) );
		scd.BufferCount = 2;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.Width = m_screenWidth;
		scd.BufferDesc.Height = m_screenHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow = m_mainWindow;
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

		// 이전까지 device, devcon, swapchain은 모두 값이 할당됐다.
		// D3D11CreateDeviceAndSwapChain()를 수행하면 m_swapChain이 null이 된다.
		// mainWindow가 존재하면 에러가 발생하지 않는다.
		if ( FAILED ( D3D11CreateDeviceAndSwapChain (
			0, driverType, 0, createDeviceFlags,
			featureLevels,
			1,	// ARRAYSIZE(featureLevels)
			D3D11_SDK_VERSION,
			&scd,
			&m_swapChain,		// out
			&m_device,			// out
			&featureLevel,	// out
			&m_devcon				// out
			) ) ) {
			cout << "D3D11CreateDeviceAndSwapChain() failed" << endl;
			return false;
		}

		ID3D11Texture2D* pBackBuffer;
		m_swapChain->GetBuffer ( 0 , IID_PPV_ARGS ( &pBackBuffer ) );
		if ( pBackBuffer ) {
			m_device->CreateRenderTargetView ( pBackBuffer , NULL , &m_renderTargetView );
			pBackBuffer->Release ();
		}
		else {
			cout << "CreateRenderTargetView() failed." << endl;
			return false;
		}

		ZeroMemory ( &m_viewport , sizeof ( D3D11_VIEWPORT ) );
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = float ( m_screenWidth );
		m_viewport.Height = float ( m_screenHeight );
		// Depth Buffering을 사용하기 위한 options
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory ( &rastDesc , sizeof ( D3D11_RASTERIZER_DESC ) );
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = false;
		m_device->CreateRasterizerState ( &rastDesc , &m_rasterizerState );

		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		depthStencilBufferDesc.Width = m_screenWidth;
		depthStencilBufferDesc.Height = m_screenHeight;
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

		if ( FAILED ( m_device->CreateTexture2D ( &depthStencilBufferDesc , 0 , m_depthStencilBuffer.GetAddressOf() ) ) ) {
			cout << "CreateTexture2D() failed" << endl;
			return false;
		}
		// 이를 통해 생성한 DepthStencilView를 이용해서 DepthStencilBuffer를 사용한다.
		if ( FAILED ( m_device->CreateDepthStencilView ( m_depthStencilBuffer.Get() , 0 , &m_depthStencilView)) ) {
			cout << "CreateDepthStencilView() failed" << endl;
			return false;
		}

		// DepthStencilView를 어떤 상태로 사용하나?
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory ( &depthStencilDesc , sizeof ( D3D11_DEPTH_STENCIL_DESC ) );
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	// depth 값이 더 작거나 같으면 화면에 그린다.
		if ( FAILED ( m_device->CreateDepthStencilState ( &depthStencilDesc , m_depthStencilState.GetAddressOf() ) ) ) {
			std::cout << "CreateDepthStencilState() failed" << std::endl;
			return false;
		}

		return true;
	}

	bool AppBase::InitGUI () {
		IMGUI_CHECKVERSION ();
		ImGui::CreateContext ();
		ImGuiIO& io = ImGui::GetIO ();
		( void ) io;
		io.DisplaySize = ImVec2 ( float ( m_screenWidth ) , float ( m_screenHeight ) );
		ImGui::StyleColorsLight ();

		// setup platform/renderer backends
		if ( !ImGui_ImplDX11_Init ( m_device.Get() , m_devcon.Get()) ) {
			return false;
		}

		if ( !ImGui_ImplWin32_Init ( m_mainWindow ) ) {
			return false;
		}

		return true;
	}

	void CheckResult ( HRESULT hr , ID3DBlob* errorBlob ) {
		if ( FAILED ( hr ) ) {
			// 파일이 없는 경우
			if ( ( hr & D3D11_ERROR_FILE_NOT_FOUND ) != 0 ) {
				cout << "File not found" << endl;
			}

			// 에러 메시지 출력
			if ( errorBlob ) {
				cout << "Shader compile error\n" << ( char* ) errorBlob->GetBufferPointer () << endl;
			}
		}
	}

	void AppBase::CreateVertexShaderAndInputLayout (
		const wstring& filename , const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements ,
		ComPtr<ID3D11VertexShader>& vertexShader , ComPtr<ID3D11InputLayout>& inputLayout
	) {
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3DCompileFromFile ( filename.c_str () , 0 , 0 , "main" , "vs_5_0" , 0 , 0 , &shaderBlob , &errorBlob );
		CheckResult ( hr , errorBlob.Get () );
		m_device->CreateVertexShader ( shaderBlob->GetBufferPointer () , shaderBlob->GetBufferSize () , NULL , &vertexShader );
		m_device->CreateInputLayout ( inputElements.data () , UINT ( inputElements.size () ) ,
			shaderBlob->GetBufferPointer () , shaderBlob->GetBufferSize () , &inputLayout );
	}

	void AppBase::CreatePixelShader ( const wstring& filename , ComPtr<ID3D11PixelShader>& pixelShader ) {
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3DCompileFromFile ( filename.c_str () , 0 , 0 , "main" , "ps_5_0" , 0 , 0 , &shaderBlob , &errorBlob );
		CheckResult ( hr , errorBlob.Get () );
		m_device->CreatePixelShader ( shaderBlob->GetBufferPointer () , shaderBlob->GetBufferSize () , NULL , &pixelShader );
	}

	void AppBase::CreateIndexBuffer ( const vector<uint16_t>& indices , ComPtr<ID3D11Buffer>& m_indexBuffer ) {
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = UINT ( sizeof ( uint16_t ) * indices.size () );
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.StructureByteStride = sizeof ( uint16_t );

		D3D11_SUBRESOURCE_DATA indexBufferData { 0 };
		indexBufferData.pSysMem = indices.data ();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		m_device->CreateBuffer ( &bufferDesc , &indexBufferData , m_indexBuffer.GetAddressOf () );
	}
}
```