- [Common Initialization](#common-initialization)
	- [1. Member Variables](#1-member-variables)
	- [2. Initialize](#2-initialize)
		- [2.1. AppBase::Initialize()](#21-appbaseinitialize)
		- [2.2. AppBase::InitMainWindow()](#22-appbaseinitmainwindow)
		- [3.3. AppBase::InitDirect3D()](#33-appbaseinitdirect3d)
		- [2.3. AppBase::InitGUI()](#23-appbaseinitgui)
	- [3. Creating Buffers](#3-creating-buffers)
		- [3.1. CreateVertexBuffer()](#31-createvertexbuffer)
		- [3.2. AppBase::CreateVertexShaderAndInputLayout()](#32-appbasecreatevertexshaderandinputlayout)
		- [3.3. AppBase::CreatePixelShader()](#33-appbasecreatepixelshader)
		- [3.4. AppBase::CreateIndexBuffer()](#34-appbasecreateindexbuffer)
		- [3.5. CreateConstantBuffer()](#35-createconstantbuffer)
		- [3.6. UpdateBuffer()](#36-updatebuffer)

# Common Initialization
**AppBase을 상속 받는 다양한 Apps는 Common Initialization을 기본으로 호출한 뒤, 각자에게 맞는 초기화를 진행**한다.   
## 1. Member Variables
```cpp
class AppBase {

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
```
모든 App에서 사용할 변수를 관리한다.   

## 2. Initialize
```cpp
class AppBase {

public:
	AppBase ();
	virtual ~AppBase ();

	virtual bool Initialize ();

	virtual LRESULT CALLBACK MsgProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam );

protected:
	bool InitMainWindow ();
	bool InitDirect3D ();
	bool InitGUI ();

};
```
window, Direct3D, imGUI를 초기화하는 기능이다.   
생성자와 소멸자, MsgProc()는 window를 위한 기능이 존재한다.   

### 2.1. AppBase::Initialize()
```cpp
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
```
순서대로 초기화를 진행한다.   
**`AppBase`를 상속 받는 자식 Apps는 해당 함수를 Init 단계에서 호출**한다.   

### 2.2. AppBase::InitMainWindow()
```cpp
AppBase* g_appBase = nullptr;

LRESULT CALLBACK WndProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam ) {
	return g_appBase->MsgProc (hwnd, msg, wParam, lParam);
}

// ...

LRESULT CALLBACK AppBase::MsgProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam ) {
	switch ( msg ) {
	case WM_SYSCOMMAND:
		if ( ( wParam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
			return 0;
	case WM_DESTROY:
		::PostQuitMessage ( 0 );
		return 0;
	}

	return DefWindowProc ( hwnd , msg , wParam , lParam );
}
```
[window programming TIP](/Note/DevTips.md/#windows-programming-부모-클래스에서-해당-인스턴스의-포인터를-전역-변수로-선언하는-이유)에서 전역 변수와 함수를 사용하는 이유를 설명했다.   
```cpp
bool AppBase::InitMainWindow () {
	WNDCLASSEX wc;
	ZeroMemory ( &wc , sizeof ( WNDCLASSEX ) );
	wc.cbSize = sizeof ( WNDCLASSEX );
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor ( NULL , IDC_ARROW );
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
```
[WindowsFramework](/Note/WindowsFramework/)를 참고한다.   

### 3.3. AppBase::InitDirect3D()
```cpp
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
		&m_swapChain, // out
		&m_device, // out
		&featureLevel, // out
		&m_devcon // out
		) ) ) {
		cout << "D3D11CreateDeviceAndSwapChain() failed" << endl;
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
	// Depth: unsigned normalized int 24bit, Stencil: unsigned int 8bit
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if ( numQualityLevels > 0 ) {
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
	}
	else {
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT; // texture memory를 어떻게 사용할 것인가?
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
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL; // depth 값이 더 작거나 같으면 화면에 그린다.
	if ( FAILED ( m_device->CreateDepthStencilState ( &depthStencilDesc , m_depthStencilState.GetAddressOf() ) ) ) {
		std::cout << "CreateDepthStencilState() failed" << std::endl;
		return false;
	}

	return true;
}
```
[InitD3DForClass.md - DirectXFramework](/Note/DirectXFramework/5_InitD3DForClass.md)를 참고한다.   

### 2.3. AppBase::InitGUI()
```cpp
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
```
[1_InitializingGUI - ImGui](/Note/ImGUI/1_InitializingGUI.md)를 참고한다.   

## 3. Creating Buffers
`AppBase`는 Vertex, Pixel, Index, Constant Buffer를 생성하는 기능을 가진다.   
모든 자식 Apps는 Buffer를 생성하여 GPU에 정보를 전달한다.   

Buffer를 생성하는 방식의 틀은 모두 비슷하다.   

### 3.1. CreateVertexBuffer()
template 함수이므로 header file에 구현한다.   
각 vertex에 대해 적용하는 shaders 프로그램을 위해서 vertex buffer를 생성한다.   
```cpp
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
```
Vertex shader에 대한 내용은 [Using Shader](/Note/DirectXFramework/4_DrawingTriangle.md/#1-using-shaders)에서 설명한다.   

### 3.2. AppBase::CreateVertexShaderAndInputLayout()
Vertex Shader를 compile하여 생성하고, Input Layout
```cpp
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
```
`ID3DBlob`은 buffer를 의미하며, 3가지 용도( Data buffers, Mesh optimization and loading, Object code and error messages )로 사용한다.   
여기서는 vertex, pixel shaders를 컴파일할 때 발생하는 error message 또는 object code를 반환한다.   

`D3DCompileFromFile()`는 [shader 파일을 컴파일하는 방법](/Note/DirectXFramework/4_DrawingTriangle.md/#1-load-and-compile-the-two-shaders-from-the-shader-file-hlsl-)에서 자세히 설명한다.   
`InputLayout`과 활용 방법은 [Input Layout?](/Note/DirectXFramework/4_DrawingTriangle.md/#2-vertex-buffer)에서 자세히 설명한다.   
Vertex Shader와 Input Layout은 관련이 있다 정도만 참고한다.   

### 3.3. AppBase::CreatePixelShader()
화면의 pixel에 대한 color를 계산하는 shaders를 생성한다.   
```cpp
void AppBase::CreatePixelShader ( const wstring& filename , ComPtr<ID3D11PixelShader>& pixelShader ) {
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = D3DCompileFromFile ( filename.c_str () , 0 , 0 , "main" , "ps_5_0" , 0 , 0 , &shaderBlob , &errorBlob );
	CheckResult ( hr , errorBlob.Get () );
	m_device->CreatePixelShader ( shaderBlob->GetBufferPointer () , shaderBlob->GetBufferSize () , NULL , &pixelShader );
}
```
pixel shader에 대한 내용은 [Using Shader](/Note/DirectXFramework/4_DrawingTriangle.md/#1-using-shaders)에서 설명한다.   

### 3.4. AppBase::CreateIndexBuffer()
```cpp
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
```

### 3.5. CreateConstantBuffer()
**model, view, projection에 적용하는 변환( Matrix )에 대한 정보를 담는 constant buffer를 생성**한다. 해당 buffer를 이용하여 shader에서 각 vertex에 matrix를 곱하여 변환을 적용한다.   

일반적으로 constant buffer에 넣을 데이터를 struct로 정의한다.   
```cpp
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
```
`T_CONSTANT`는 각 자식 Apps에서 관리하는 constant buffer에 넘기는 구조체를 의미한다.   
```cpp
// ConstantBuffer로 보낼 데이터
struct ModelViewProjectionConstantBuffer {
	Matrix model;
	Matrix view;
	Matrix projection;
};
```
위 코드는 `Cube.h`에 정의한 constant buffer 데이터 구조체다.   
model, view, projection에 적용할 변환( Matrix )을 관리한다.   

### 3.6. UpdateBuffer()
**constant buffer data를 CPU에서 GPU로 복사하는 역할을 수행**한다.   
```cpp
template <typename T_DATA>
void UpdateBuffer ( const T_DATA& bufferData , ComPtr<ID3D11Buffer>& buffer ) {
	D3D11_MAPPED_SUBRESOURCE ms;
	m_devcon->Map ( buffer.Get () , NULL , D3D11_MAP_WRITE_DISCARD , NULL , &ms );
	memcpy ( ms.pData , &bufferData , sizeof ( bufferData ) );
	m_devcon->Unmap ( buffer.Get () , NULL );
}
```
[Filling the vertex buffer](/Note/DirectXFramework/4_DrawingTriangle.md/#3-filling-the-vertex-buffer)에서 `Map`과 `UnMap`에 대한 설명이 존재한다.   
자식 Apps에서 매 프레임마다 변환( 애니메이션 )을 적용하는 `Update()`에서 이를 마지막에 호출한다.   