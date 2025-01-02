#include "CubeApp.h"

#include <tuple>

namespace pt {

using namespace std;

auto MakeBox () {

  vector<Vector3> positions;
  vector<Vector3> colors;
  vector<Vector3> normals;

  const float scale = 1.0f;

  // 윗면
  positions.push_back ( Vector3 ( -1.0f , 1.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( -1.0f , 1.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 1.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 1.0f , -1.0f ) * scale );
  colors.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );

  // 아랫면
  positions.push_back ( Vector3 ( -1.0f , 0.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( -1.0f , 0.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 0.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 0.0f , -1.0f ) * scale );
  colors.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 0.0f , 1.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , -1.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , -1.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , -1.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , -1.0f , 0.0f ) );

  // 앞면
  positions.push_back ( Vector3 ( -1.0f , 1.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 1.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 0.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( -1.0f , 0.0f , -1.0f ) * scale );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , -1.0f ) );


  // 뒷면
  positions.push_back ( Vector3 ( -1.0f , 1.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 1.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 0.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( -1.0f , 0.0f , 1.0f ) * scale );
  colors.push_back ( Vector3 ( 0.5f , 0.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 0.5f , 0.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 0.5f , 0.0f , 0.0f ) );
  colors.push_back ( Vector3 ( 0.5f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );
  normals.push_back ( Vector3 ( 0.0f , 0.0f , 1.0f ) );

  // 왼쪽
  positions.push_back ( Vector3 ( 1.0f , 1.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 1.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 0.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( 1.0f , 0.0f , -1.0f ) * scale );
  colors.push_back ( Vector3 ( 0.0f , 0.5f , 0.25f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.5f , 0.25f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.5f , 0.25f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.5f , 0.25f ) );
  normals.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( 1.0f , 0.0f , 0.0f ) );

  // 오른쪽
  positions.push_back ( Vector3 ( -1.0f , 1.0f , 1.0f ) * scale );
  positions.push_back ( Vector3 ( -1.0f , 1.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( -1.0f , 0.0f , -1.0f ) * scale );
  positions.push_back ( Vector3 ( -1.0f , 0.0f , 1.0f ) * scale );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 0.25f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 0.25f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 0.25f ) );
  colors.push_back ( Vector3 ( 0.0f , 0.0f , 0.25f ) );
  normals.push_back ( Vector3 ( -1.0f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( -1.0f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( -1.0f , 0.0f , 0.0f ) );
  normals.push_back ( Vector3 ( -1.0f , 0.0f , 0.0f ) );

  vector<Vertex> vertices;
  for ( size_t i = 0; i < positions.size (); i++ ) {
    Vertex v;
    v.position = positions[ i ];
    v.color = colors[ i ];
    vertices.push_back ( v );
  }

  vector<uint16_t> indices = {
      0,  1,  2,  0,  2,  3,  // 윗면
      4, 5, 6, 4, 6, 7, // 아랫면
      8, 9, 10, 8, 10, 11, // 앞면
      12, 13, 14, 12, 14, 15, // 뒷면
      16, 17, 18, 16, 18, 19, // 왼쪽
      20, 21, 22, 20, 22, 23, // 오른쪽
  };

  return tuple{ vertices, indices };
}

CubeApp::CubeApp () : AppBase () , m_indexCount ( 0 ) {}

bool CubeApp::Initialize () {
  if ( !AppBase::Initialize () ) {
    return false;
  }

  auto [vertices , indices] = MakeBox ();

  // Creating Buffers
  AppBase::CreateVertexBuffer ( vertices , m_vertexBuffer );
  m_indexCount = UINT ( indices.size () );
  AppBase::CreateIndexBuffer ( indices , m_indexBuffer );
  m_constantBufferData.model = Matrix ();
  m_constantBufferData.view = Matrix ();
  m_constantBufferData.projection = Matrix ();
  AppBase::CreateConstantBuffer ( m_constantBufferData , m_constantBuffer );

  // Creating Shaders
  vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };
  AppBase::CreateVertexShaderAndInputLayout ( L"CubeVertexShader.hlsl" , inputElements , m_colorVertexShader , m_colorInputLayout );
  AppBase::CreatePixelShader ( L"CubePixelShader.hlsl" , m_colorPixelShader );

  return true;
}

void CubeApp::UpdateGUI () {
  ImGui::Checkbox ( "usePerspectiveProjection" , &m_usePerspectiveProjection );
}

void CubeApp::Update ( float dt )
{
  // 시간이 흐르면서 Cube를 회전시킨다.
  static float rot = 0.0f;
  rot += dt;

  // model 및 view 변환
  m_constantBufferData.model = Matrix::CreateScale ( 0.5f ) * Matrix::CreateRotationY ( rot ) *
    Matrix::CreateTranslation ( Vector3 ( 0.0f , -0.3f , 1.0f ) );
  m_constantBufferData.model = m_constantBufferData.model.Transpose ();

  using namespace DirectX;
  m_constantBufferData.view = XMMatrixLookAtLH ( { 0.0f, 1.0f, -1.0f } , { 0.0f, 0.0f, 1.0f } , { 0.0f, 1.0f, 0.0f } );
  m_constantBufferData.view = m_constantBufferData.view.Transpose ();

  // projection
  const float aspect = AppBase::GetAspectRatio ();
  if ( m_usePerspectiveProjection ) {
    const float fovAngleY = 70.0f * XM_PI / 180.0f;
    m_constantBufferData.projection = XMMatrixPerspectiveFovLH ( fovAngleY , aspect , 0.01f , 100.0f );
  }
  else {
    m_constantBufferData.projection = XMMatrixOrthographicOffCenterLH ( -aspect , aspect , -1.0f , 1.0f , 0.1f , 10.0f );
  }
  m_constantBufferData.projection = m_constantBufferData.projection.Transpose ();

  // constant를 CPU에서 GPU로 복사
  AppBase::UpdateBuffer ( m_constantBufferData , m_constantBuffer );
}

void CubeApp::Render ()
{
  // Graphics Pipeline을 이용하여 Rendering한다.
  // IA, VS, PS, RS, OM stage가 존재한다.
  // 함수 실행 순서와 Pipeline 순서와 동일하지 않다.
  
  // RS: Rasterization Stage( 3D coordinates(world) -> 2D coordinates(screen) )
  m_devcon->RSSetViewports ( 1 , &m_viewport );

  // 매 프레임마다 이전 프레임을 지운다.
  // render target은 2차원 배열( 화면 )값이므로 이를 clear color 값으로 채운다.
  float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
  m_devcon->ClearRenderTargetView ( m_renderTargetView.Get () , clearColor );
  m_devcon->ClearDepthStencilView ( m_depthStencilView.Get () ,
    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL , 1.0f , 0 );

  // OM: Output-Merger Stage
  // m_devcon->OMSetRenderTargets ( 1 , m_renderTargetView.GetAddressOf () , nullptr ); DepthBuffer 사용X
  m_devcon->OMSetRenderTargets ( 1 , m_renderTargetView.GetAddressOf () , m_depthStencilView.Get () );
  m_devcon->OMSetDepthStencilState ( m_depthStencilState.Get () , 0 );

  // 어떤 shader를 사용할지
  m_devcon->VSSetShader ( m_colorVertexShader.Get () , 0 , 0 );
  m_devcon->VSSetConstantBuffers ( 0 , 1 , m_constantBuffer.GetAddressOf () ); // VS에서 사용할 cb를 선택
  m_devcon->PSSetShader ( m_colorPixelShader.Get () , 0 , 0 );

  // RS에서 어떤 state를 사용할지
  m_devcon->RSSetState ( m_rasterizerState.Get () );

  // vertex, index buffer 설정
  UINT stride = sizeof ( Vertex );
  UINT offset = 0;
  m_devcon->IASetInputLayout ( m_colorInputLayout.Get () );
  m_devcon->IASetVertexBuffers ( 0 , 1 , m_vertexBuffer.GetAddressOf () , &stride , &offset );
  m_devcon->IASetIndexBuffer ( m_indexBuffer.Get () , DXGI_FORMAT_R16_UINT , 0 );
  m_devcon->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

  // Rendering 준비가 완료되면, GPU가 Rendering을 시작
  m_devcon->DrawIndexed ( m_indexCount , 0 , 0 );
}

}	// namespace pt