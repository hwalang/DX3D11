#pragma once

#include <algorithm>
#include <memory>

#include "AppBase.h"

namespace pt {

class CubeApp : public AppBase {
public:
	CubeApp ();

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
	UINT m_indexCount;

	ModelViewProjectionConstantBuffer m_constantBufferData;

	bool m_usePerspectiveProjection = true;
};
}	// namespace pt