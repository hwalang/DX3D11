- [Introduce](#introduce)
- [SquareApp::Initialize()](#squareappinitialize)
  - [1. MakeSquare()](#1-makesquare)
    - [1.1. NDC와 UV의 매핑 방법](#11-ndc와-uv의-매핑-방법)
- [Square::UpdateGUI()](#squareupdategui)
  - [HLSL](#hlsl)
- [AppBase::Run()](#appbaserun)
  - [1. Square::Update()](#1-squareupdate)
  - [2. Square::Render()](#2-squarerender)
- [SquareApp.h](#squareapph)
- [SquareApp.cpp](#squareappcpp)
- [SquareVertexShader.hlsl](#squarevertexshaderhlsl)
- [SquarePixelShader.hlsl](#squarepixelshaderhlsl)

# Introduce
Square를 Rendering한 화면을 띄우는 App.   
GUI를 이용하여 Square의 color를 실시간으로 변경할 수 있다.   

이를 구현하기 위해서 [Texture Coordinates](https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates)를 활용한다.   
[pixel과 texel coordinate system](https://learn.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates)을 살펴보는 것도 추천한다.   

# SquareApp::Initialize()
```cpp
bool Square::Initialize () {
	if ( !AppBase::Initialize () ) {
		return false;
	}
	auto [vertices , indices] = MakeSqure ();

	AppBase::CreateVertexBuffer ( vertices , m_vertexBuffer );
	m_indexCount = UINT ( indices.size () );
	AppBase::CreateIndexBuffer ( indices , m_indexBuffer );
	m_constantBufferData.model = Matrix ();
	m_constantBufferData.view = Matrix ();
	m_constantBufferData.projection = Matrix ();
	AppBase::CreateConstantBuffer ( m_constantBufferData , m_constantBuffer );
	AppBase::CreateConstantBuffer ( m_pixelShaderConstantBufferData , m_pixelShaderConstantBuffer );
	
	// input layout은 Vertex struct의 순서와 형식에 맞추면 된다.
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	AppBase::CreateVertexShaderAndInputLayout ( L"SquareVertexShader.hlsl" , inputElements , m_colorVertexShader , m_colorInputLayout );
	AppBase::CreatePixelShader ( L"SquarePixelShader.hlsl" , m_colorPixelShader );

	return true;
}
```
`MakeSquare()`로 2D 정사각형에 대한 정보( vertices, indices )를 세팅한다.   
VertexBuffer, IndexBuffer, ConstantBuffer를 생성한다. CubeApp과 다른 점은 PixelShader의 ConstantBuffer도 추가로 생성했다.   
Vertex Struct 구조에 맞게 Input Layout을 설정하고, Shader를 생성한다.   

## 1. MakeSquare()
```cpp
auto MakeSqure () {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords;

	const float scale = 1.0f;

	positions.push_back ( Vector3 ( -1.0f , 1.0f , 0.0f ) * scale );
	positions.push_back ( Vector3 ( 1.0f , 1.0f , 0.0f ) * scale );
	positions.push_back ( Vector3 ( 1.0f , -1.0f , 0.0f ) * scale );
	positions.push_back ( Vector3 ( -1.0f , -1.0f , 0.0f ) * scale );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );

	// 사각형의 각 vertex와 texel 좌표를 올바른 UV에 배정한다.
	texcoords.push_back ( Vector2 ( 0.0f , 0.0f ) );
	texcoords.push_back ( Vector2 ( 1.0f , 0.0f ) );
	texcoords.push_back ( Vector2 ( 1.0f , 1.0f ) );
	texcoords.push_back ( Vector2 ( 0.0f , 1.0f ) );

	vector<Vertex> vertices;
	for ( size_t i = 0; i < positions.size (); i++ ) {
		Vertex v;
		v.position = positions[ i ];
		v.color = colors[ i ];
		v.texcoord = texcoords[ i ];
		vertices.push_back ( v );
	}
	vector<uint16_t> indices = {
			0, 1, 2, 0, 2, 3,
	};

	return tuple{ vertices, indices };
}
```
vertices에 texcoord 좌표를 저장한다.   
**vertex shader에 texcoord 좌표를 넘겨야 pixel shader에도 texcoord 좌표가 입력**된다.   

### 1.1. NDC와 UV의 매핑 방법
[NDC와 UV매핑에 대한 설명 - MSLearn](https://learn.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates)   

`texcoords`에 각 UV 좌표를 세팅한다.   

**중요한 점은 각 UV 좌표를 의도한 vertex에 mapping 해야 한다**.   
![alt text](Images/SquareApp/MappingUVtoNDC.png)   
vertex에 대한 변환은 GPU의 shader가 담당하기 때문에 `positions`의 좌표는 추가적인 변환( world, view, projection )이 없이 NDC 좌표를 의미한다.   
이러한 NDC 좌표에 맞춰서 UV 좌표를 매핑하는데, NDC의 왼쪽위(-1, 1)에 UV의 왼쪽위(0, 0)을 매핑하는 방식으로 올바른 위치에 배정한다.   
즉, **`positions`를 NDC 범위로 잡고, 해당 vertices를 Uv 좌표에 일대일로 대응**시킨다.   

# Square::UpdateGUI()
```cpp
void Square::UpdateGUI () {
	ImGui::SliderFloat ( "xSplit" , &m_pixelShaderConstantBufferData.xSplit , 0.0f , 1.0f );
}
```
GUI의 slider bar를 이용하여 square의 색깔을 변경하기 위함이다.   
slider bar를 이용하여 `PixelShaderConstantBuffer`의 `xSplit`값을 0.0 ~ 1.0f로 변경한다.   
이렇게 변경한 값을 이용하여 pixel shader에서 square의 색깔을 변경시킨다.   

## HLSL
vertex shader에서는 texcoord 값을 수정하지 않고 그대로 pixel shader에 넘긴다.   
```cpp
// PixelShader
cbuffer PixelShaderConstantBuffer : register(b0) {
    float xSplit;
};

struct PixelInput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET {
    // blue or red
    return input.texcoord.x > xSplit ? float4(0.0, 0.0, 1.0, 1.0) : float4(1.0, 0.0, 0.0, 1.0f);
}
```
`main`에서 `texcoord.x` 값이 `xSplit`보다 크면 blue를 아니면 red로 square를 색칠한다.   
즉, `x = 0 ~ 1` 함수를 기준으로 square를 잘라서 양쪽의 색깔을 다르게 표현한다.   

# AppBase::Run()

## 1. Square::Update()
```cpp
void Square::Update ( float dt ) {
	using namespace DirectX;

	// model transformation
	m_constantBufferData.model = Matrix::CreateScale ( 1.5f ) * Matrix::CreateTranslation ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	m_constantBufferData.model = m_constantBufferData.model.Transpose ();

	// view transformation
	const Vector3 viewEyePos = { 0.0f, 0.0f, -2.0f };
	const Vector3 viewEyeDir = { 0.0f, 0.0f, 1.0f };
	const Vector3 viewUp = { 0.0f, 1.0f, 0.0f };
	m_constantBufferData.view = XMMatrixLookToLH ( viewEyePos , viewEyeDir , viewUp );
	m_constantBufferData.view = m_constantBufferData.view.Transpose ();

	// projection
	const float aspect = AppBase::GetAspectRatio ();
	const float projFovAngleY = 70.0f;
	const float nearZ = 0.01f;
	const float farZ = 100.0f;
	if ( m_usePerspectiveProjection ) {
		m_constantBufferData.projection = XMMatrixPerspectiveFovLH (
			XMConvertToRadians ( projFovAngleY ) ,
			aspect , nearZ , farZ );
	}
	else {
		m_constantBufferData.projection = XMMatrixOrthographicOffCenterLH (
			-aspect , aspect , -1.0f , 1.0f , nearZ , farZ );
	}
	m_constantBufferData.projection = m_constantBufferData.projection.Transpose ();

	// shader에서 사용할 constant buffer를 CPU to GPU
	AppBase::UpdateBuffer ( m_constantBufferData , m_constantBuffer );
	AppBase::UpdateBuffer ( m_pixelShaderConstantBufferData , m_pixelShaderConstantBuffer );
}
```
CubeApp과 다른 점은 PixelShader에서 사용할 ConstantBuffer를 GPU에 넘겨준 것이다.   

## 2. Square::Render()
```cpp
void Square::Render () {
	m_devcon->RSSetViewports ( 1 , &m_viewport );

	float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_devcon->ClearRenderTargetView ( m_renderTargetView.Get () , clearColor );
	m_devcon->ClearDepthStencilView ( m_depthStencilView.Get () ,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL , 1.0f , 0 );

	m_devcon->OMSetRenderTargets ( 1 , m_renderTargetView.GetAddressOf () , m_depthStencilView.Get () );
	m_devcon->OMSetDepthStencilState ( m_depthStencilState.Get () , 0 );

	// Set Shader
	m_devcon->VSSetShader ( m_colorVertexShader.Get () , 0 , 0 );
	m_devcon->VSSetConstantBuffers ( 0 , 1 , m_constantBuffer.GetAddressOf () );
	m_devcon->PSSetShader ( m_colorPixelShader.Get () , 0 , 0 );
	m_devcon->PSSetConstantBuffers ( 0 , 1 , m_pixelShaderConstantBuffer.GetAddressOf () );

	m_devcon->RSSetState ( m_rasterizerState.Get () );

	UINT stride = sizeof ( Vertex );
	UINT offset = 0;
	m_devcon->IASetInputLayout ( m_colorInputLayout.Get () );
	m_devcon->IASetVertexBuffers ( 0 , 1 , m_vertexBuffer.GetAddressOf () , &stride , &offset );
	m_devcon->IASetIndexBuffer ( m_indexBuffer.Get () , DXGI_FORMAT_R16_UINT , 0 );
	m_devcon->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_devcon->DrawIndexed ( m_indexCount , 0 , 0 );
}
```
PixelShader에서 사용할 Constant Buffer를 세팅한다.   

# SquareApp.h
```cpp
 #pragma once

#include "AppBase.h"

namespace pt {

// pixel에 다양한 효과를 주는 constant buffer
struct PixelShaderConstantBuffer {
	float xSplit;       // 4 bytes
	float padding[ 3 ]; // 12 bytes
};

static_assert( ( sizeof ( PixelShaderConstantBuffer ) % 16 == 0 ) ,
	"[PS] Constant Buffer size must be 16-bytes aligend" );

class Square : public AppBase {
public:
	Square ();

	virtual bool Initialize () override;
	virtual void UpdateGUI () override;
	virtual void Update ( float dt ) override;
	virtual void Render () override;

protected:
	ComPtr<ID3D11VertexShader> m_colorVertexShader;
	ComPtr<ID3D11PixelShader> m_colorPixelShader;
	ComPtr<ID3D11InputLayout> m_colorInputLayout;

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	ComPtr<ID3D11Buffer> m_constantBuffer;
	ComPtr<ID3D11Buffer> m_pixelShaderConstantBuffer;
	UINT m_indexCount;

	ModelViewProjectionConstantBuffer m_constantBufferData;
	PixelShaderConstantBuffer m_pixelShaderConstantBufferData;

	bool m_usePerspectiveProjection = true;
};

} // namespace pt
```
# SquareApp.cpp
```cpp
#include "SquareApp.h"

#include <tuple>

namespace pt {

using namespace std;

auto MakeSqure () {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords;

	const float scale = 1.0f;

	positions.push_back ( Vector3 ( -1.0f , 1.0f , 0.0f ) * scale );
	positions.push_back ( Vector3 ( 1.0f , 1.0f , 0.0f ) * scale );
	positions.push_back ( Vector3 ( 1.0f , -1.0f , 0.0f ) * scale );
	positions.push_back ( Vector3 ( -1.0f , -1.0f , 0.0f ) * scale );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
	normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );

	texcoords.push_back ( Vector2 ( 0.0f , 0.0f ) );
	texcoords.push_back ( Vector2 ( 1.0f , 0.0f ) );
	texcoords.push_back ( Vector2 ( 1.0f , 1.0f ) );
	texcoords.push_back ( Vector2 ( 0.0f , 1.0f ) );

	vector<Vertex> vertices;
	for ( size_t i = 0; i < positions.size (); i++ ) {
		Vertex v;
		v.position = positions[ i ];
		v.color = colors[ i ];
		v.texcoord = texcoords[ i ];
		vertices.push_back ( v );
	}
	vector<uint16_t> indices = {
			0, 1, 2, 0, 2, 3,
	};

	return tuple{ vertices, indices };
}

Square::Square () : AppBase(), m_indexCount(0) {}

bool Square::Initialize () {
	if ( !AppBase::Initialize () ) {
		return false;
	}

	auto [vertices , indices] = MakeSqure ();

	AppBase::CreateVertexBuffer ( vertices , m_vertexBuffer );
	m_indexCount = UINT ( indices.size () );
	AppBase::CreateIndexBuffer ( indices , m_indexBuffer );
	m_constantBufferData.model = Matrix ();
	m_constantBufferData.view = Matrix ();
	m_constantBufferData.projection = Matrix ();
	AppBase::CreateConstantBuffer ( m_constantBufferData , m_constantBuffer );
	AppBase::CreateConstantBuffer ( m_pixelShaderConstantBufferData , m_pixelShaderConstantBuffer );
	
	// input layout은 Vertex struct의 순서와 형식에 맞추면 된다.
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	AppBase::CreateVertexShaderAndInputLayout ( L"SquareVertexShader.hlsl" , inputElements , m_colorVertexShader , m_colorInputLayout );
	AppBase::CreatePixelShader ( L"SquarePixelShader.hlsl" , m_colorPixelShader );

	return true;
}

void Square::UpdateGUI () {
	ImGui::SliderFloat ( "xSplit" , &m_pixelShaderConstantBufferData.xSplit , 0.0f , 1.0f );
}

void Square::Update ( float dt ) {
	using namespace DirectX;

	// model transformation
	m_constantBufferData.model = Matrix::CreateScale ( 1.5f ) * Matrix::CreateTranslation ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
	m_constantBufferData.model = m_constantBufferData.model.Transpose ();

	// view transformation
	const Vector3 viewEyePos = { 0.0f, 0.0f, -2.0f };
	const Vector3 viewEyeDir = { 0.0f, 0.0f, 1.0f };
	const Vector3 viewUp = { 0.0f, 1.0f, 0.0f };
	m_constantBufferData.view = XMMatrixLookToLH ( viewEyePos , viewEyeDir , viewUp );
	m_constantBufferData.view = m_constantBufferData.view.Transpose ();

	// projection
	const float aspect = AppBase::GetAspectRatio ();
	const float projFovAngleY = 70.0f;
	const float nearZ = 0.01f;
	const float farZ = 100.0f;
	if ( m_usePerspectiveProjection ) {
		m_constantBufferData.projection = XMMatrixPerspectiveFovLH (
			XMConvertToRadians ( projFovAngleY ) ,
			aspect , nearZ , farZ );
	}
	else {
		m_constantBufferData.projection = XMMatrixOrthographicOffCenterLH (
			-aspect , aspect , -1.0f , 1.0f , nearZ , farZ );
	}
	m_constantBufferData.projection = m_constantBufferData.projection.Transpose ();

	// shader에서 사용할 constant buffer를 CPU to GPU
	AppBase::UpdateBuffer ( m_constantBufferData , m_constantBuffer );
	AppBase::UpdateBuffer ( m_pixelShaderConstantBufferData , m_pixelShaderConstantBuffer );
}

void Square::Render () {
	m_devcon->RSSetViewports ( 1 , &m_viewport );

	float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_devcon->ClearRenderTargetView ( m_renderTargetView.Get () , clearColor );
	m_devcon->ClearDepthStencilView ( m_depthStencilView.Get () ,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL , 1.0f , 0 );

	m_devcon->OMSetRenderTargets ( 1 , m_renderTargetView.GetAddressOf () , m_depthStencilView.Get () );
	m_devcon->OMSetDepthStencilState ( m_depthStencilState.Get () , 0 );

	// Set Shader
	m_devcon->VSSetShader ( m_colorVertexShader.Get () , 0 , 0 );
	m_devcon->VSSetConstantBuffers ( 0 , 1 , m_constantBuffer.GetAddressOf () );
	m_devcon->PSSetShader ( m_colorPixelShader.Get () , 0 , 0 );
	m_devcon->PSSetConstantBuffers ( 0 , 1 , m_pixelShaderConstantBuffer.GetAddressOf () );

	m_devcon->RSSetState ( m_rasterizerState.Get () );

	UINT stride = sizeof ( Vertex );
	UINT offset = 0;
	m_devcon->IASetInputLayout ( m_colorInputLayout.Get () );
	m_devcon->IASetVertexBuffers ( 0 , 1 , m_vertexBuffer.GetAddressOf () , &stride , &offset );
	m_devcon->IASetIndexBuffer ( m_indexBuffer.Get () , DXGI_FORMAT_R16_UINT , 0 );
	m_devcon->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_devcon->DrawIndexed ( m_indexCount , 0 , 0 );
}



} // namespace pt
```
# SquareVertexShader.hlsl
```cpp
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

struct VertexInput
{
    float3 pos : POSITION;
    float3 color : COLOR0;
    float2 texcoord : TEXCOORD0; // pixel shader로 넘겨주기 위해서 vertex shader에서 texcoord를 받는다.
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

PixelInput main(VertexInput input)
{
    PixelInput output;
    float4 pos = float4(input.pos, 1.0f);
    
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;
    output.color = input.color;
    output.texcoord = input.texcoord;
    
    return output;
}
```
# SquarePixelShader.hlsl
```cpp
cbuffer PixelShaderConstantBuffer : register(b0)
{
    float xSplit;
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    // blue or red
    return input.texcoord.x > xSplit ? float4(0.0, 0.0, 1.0, 1.0) : float4(1.0, 0.0, 0.0, 1.0f);
}
```