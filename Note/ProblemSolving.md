- [삼각형이 잘리는 경우( Clipping )](#삼각형이-잘리는-경우-clipping-)
	- [ME](#me)
	- [GPT](#gpt)
	- [Summary](#summary)
- [Pixel Shader를 Vertex Shader로 인식하는 경우](#pixel-shader를-vertex-shader로-인식하는-경우)

<br>

# 삼각형이 잘리는 경우( Clipping )

<div>
	<img src="Images/ProblemSolving/DrawingTriangleError.png" width="45%" />
	<img src="Images/ProblemSolving/DrawingTriangleSolving.png" width="45%" />
</div>

**[ before | after ]**   

[DrawingTriangle.md - Final Code](DirectXFramework/4_DrawingTriangle.md/#final-code)를 참고한다.   
삼각형이 화면에 짤려서 나오는 경우가 있었다.   

## ME
해당 문제가 발생했을 때, NDC 좌표에서 pixel coordinates로 변환하는 과정에서 viewport 설정에 문제가 없는지 살펴봤다.   
```cpp
VERTEX OurVertices[] = {
	{ 0.0f, 0.5f, 0.0f, D3D11_VIDEO_COLOR_RGBA ( 1.0f, 0.0f, 0.0f, 1.0f ) },
	{ 0.45f, -0.5f, 0.0f, D3D11_VIDEO_COLOR_RGBA ( 0.0f, 1.0f, 0.0f, 1.0f ) },
	{ -0.45f, -0.5f, 0.0f, D3D11_VIDEO_COLOR_RGBA ( 0.0f, 0.0f, 1.0f, 1.0f ) }
};
```
삼각형의 좌표는 NDC를 기준( -1, 1 )으로 작성됐기 때문이다.   
이러한 좌표계라면 이론적으로 삼각형이 Clipping 없이 화면에 잘 나타나야 한다.   
그래서 viewport 코드를 살펴봤다.   
```cpp
// Set the viewport
D3D11_VIEWPORT viewPort;
ZeroMemory ( &viewPort , sizeof ( D3D11_VIEWPORT ) );
viewPort.TopLeftX = 0;
viewPort.TopLeftY = 0;
viewPort.Width = SCREEN_WIDTH;
viewPort.Height = SCREEN_HEIGHT;
devcon->RSSetViewports ( 1 , &viewPort );
```
하지만 해당 코드에서는 문제가 없다.   

## GPT
GPT는 input layout과 vertex data의 정의가 불일치함을 지적했다.   
```cpp
// create the input layout object
D3D11_INPUT_ELEMENT_DESC ied[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```
삼각형의 좌표( POSITION )은 (X, Y, Z)인 3개의 float만 가지지만, input layout은 4개의 float를 가지고 있기 때문이다.   
즉, 4개의 float를 읽어들이는 `R32G32B32A32_FLOAT`는 실제 데이터가 3개의 float로 정의된 경우, 데이터 해석이 잘못될 가능성이 있다.   
```cpp
// create the input layout object
D3D11_INPUT_ELEMENT_DESC ied[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```
POSITION 속성을 3개의 float로 읽어들이도록 설정하니 해결됐다.   

## Summary
```
[ 문제 원인 ]
input layout과 vertex data의 불일치로 인해 정점 데이터가 잘못 해석되어 그려진 삼각형에 Clipping이 발생했다.

[ 해결 방법 ]
input layout의 POSITION property를 DXGI_FORMAT_R32G32B32_FLOAT로 수정하여 두 정의를 일치시켰다.
```

# Pixel Shader를 Vertex Shader로 인식하는 경우
compiler가 pixel shader file을 vertex shader file로 인식하여 `Invalid vs_2_0 output semantic SV_Target` 에러가 발생했다.   

![alt text](Images/ProblemSolving/HLSL_Compiler_Shader_Type.png)   

[참고 자료](https://stackoverflow.com/questions/45422730/error-invalid-vs-2-0-output-semantic)   
**pixel shader file의 properties를 수정**한다.   