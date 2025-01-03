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
#include <directxtk/SimpleMath.h>

namespace pt {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using Microsoft::WRL::ComPtr;
using std::vector;
using std::wstring;

// pixel shader로 다양한 효과를 부여하려면
// 이를 canvas처럼 활용할 수 있는 texture coordinates가 필요하다.
struct Vertex {
	Vector3 position;
	Vector3 color;
	Vector2 texcoord; // texture coordinates
};

struct ModelViewProjectionConstantBuffer {
	Matrix model;
	Matrix view;
	Matrix projection;
};

static_assert( ( sizeof ( ModelViewProjectionConstantBuffer ) % 16 == 0 ) ,
	"[MVP] Constant Buffer size must be 16-bytes aligend" );

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
	void CreateTexture(const std::string filename, 
		ComPtr<ID3D11Texture2D>& texture , ComPtr<ID3D11ShaderResourceView>& textureResourceView );

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