- [DirectXMath](#directxmath)
  - [1. XMVECTOR, XMFLOAT](#1-xmvector-xmfloat)
  - [2. SIMD](#2-simd)
- [SimpleMath](#simplemath)
  - [1. Matrix, Vector](#1-matrix-vector)
  - [2. Matrix Multiplication](#2-matrix-multiplication)

<BR>
<BR>

`row-major matrix`를 사용하기 때문에 `먼저 적용해야 하는 행렬이 가장 앞에 위치`한다.   

# DirectXMath
```cpp
// main
if (!XMVerifyCPUSupport()) {
  cout << "directx math not supported" << endl;
  return -1;
}
```
현재 컴퓨터가 DirectXMath를 지원하는지 확인한다.   

## 1. XMVECTOR, XMFLOAT
```cpp
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

XMVECTOR          // using local or global variable
XMFLOAT2, 3, 4    // using class member variable
```
먼저 `XMVECTOR`와 `XMFLOAT2, 3, 4`를 구분해서 사용한다.   
1. vector를 local 또는 global variable을 선언하여 사용할 때는 `XMVECTOR`를 사용
2. vector를 class member variable로 사용할 때는 `XMFLOAT`를 사용
3. `XMFLOAT는 데이터를 저장`할 때 사용하고, 이를 이용해서 `연산을 할 때는 XMVECTOR로 변환`해서 연산을 수행

```cpp
XMFLOAT4 xfloat4 = {1.0f, 2.0f, 3.0f, 1.0f};
XMVECTOR xvector = XMLoadFloat4(&xfloat4);
xvector = XMVector3Length(xvector);
XMStoreFloat(&length, xvector);     // length는 특정 값
```

## 2. SIMD
XMVECTOR와 XMFLOAT를 구분하서 사용하는 이유는 `SIMD 가속`을 사용하기 위함이다.   

<BR>
<BR>

# SimpleMath
DirectXMath는 사용하기엔 복잡하다.   
따라서 간접적으로 DirectXMath를 이용하는 SimpleMath를 사용하는 것을 권장한다.   

## 1. Matrix, Vector
```cpp
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

Matrix tr = Matrix::CreateTranslation(Vector3(1.0f, 2.0f, 3.0f));   // glm의 translate()와 같다
Vector4 myPoint(4.0f, 5.0f, 6.0f, 1.0f);
Vector4 myVector(4.0f, 5.0f, 6.0f, 0.0f);
```
Matrix를 타고 들어가면 XMFLOAT4X4를 상속받음을 알 수 있다.   
내부적으로 XMVECTOR와 XMFLOAT의 변환 과정을 수행하기 때문에 간단하다.   

## 2. Matrix Multiplication
```cpp
myPoint = Vector4::Transform(myPoint, tr);
myVector = Vector4::Transform(myVector, tr);
```