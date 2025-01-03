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

	// Create Texture, ShaderResourceView, SamplerState
	AppBase::CreateTexture ( "crate2_diffuse.png" , m_texture , m_textureResourceView );
	AppBase::CreateTexture ( "wall.jpg" , m_texture2 , m_textureResourceView2 );
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory ( &sampDesc , sizeof ( sampDesc ) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 일반적으로 사용: POINT, LINEAR interpolation
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// CLAMP, 
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_device->CreateSamplerState ( &sampDesc , m_samplerState.GetAddressOf () );

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

	ID3D11ShaderResourceView* pixelResources[ 2 ] = { m_textureResourceView.Get (), m_textureResourceView2.Get() };
	m_devcon->PSSetShaderResources ( 0 , 2 , pixelResources );
	m_devcon->PSSetSamplers ( 0 , 1 , m_samplerState.GetAddressOf () );
	m_devcon->PSSetConstantBuffers ( 0 , 1 , m_pixelShaderConstantBuffer.GetAddressOf () );
	m_devcon->PSSetShader ( m_colorPixelShader.Get () , 0 , 0 );

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
