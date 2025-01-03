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

	// Texturing
	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11Texture2D> m_texture2;
	ComPtr<ID3D11ShaderResourceView> m_textureResourceView;
	ComPtr<ID3D11ShaderResourceView> m_textureResourceView2;
	ComPtr<ID3D11SamplerState> m_samplerState;

	ModelViewProjectionConstantBuffer m_constantBufferData;
	PixelShaderConstantBuffer m_pixelShaderConstantBufferData;

	bool m_usePerspectiveProjection = true;
};

} // namespace pt