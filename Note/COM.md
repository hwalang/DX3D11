- [Component Object Model( COM )](#component-object-model-com-)
  - [COM object에 대해 꼭 알아야 할 네 가지 사항](#com-object에-대해-꼭-알아야-할-네-가지-사항)
- [ComPtr](#comptr)
  - [1. Initialize](#1-initialize)
  - [2. ETC](#2-etc)
    - [FAILED() macro](#failed-macro)
    - [As() | Get(), GetAddressOf()](#as--get-getaddressof)
    - [Reset()](#reset)

<br><br>

[MSLearn - ComPtr Class](https://learn.microsoft.com/en-us/cpp/cppcx/wrl/comptr-class?view=msvc-170)   
[GitHub - ComPtr ](https://github.com/Microsoft/DirectXTK/wiki/ComPtr#initialization)   
[DirectXTutorial - Initializing Direct3D](http://www.directxtutorial.com/Lesson.aspx?lessonid=11-4-2)   

# Component Object Model( COM )
**레고처럼 많은 역할을 수행하는 advanced objects를 만드는 방법**이다.   
레고는 advanced shapes를 만들기 위해 서로 붙는다. 레고로 만들어진 shape 내의 각 레고들은 서로를 신경쓰지 않는다.   
레고는 모두 서로 호환( compatible )되며, 우리는 레고가 작동하도록 서로 조립하면 된다. 만약 하나의 조각을 바꾸고 싶다면, 이를 제거한 뒤 다른 레고를 붙이면 된다.   

**Direct3D는 COM objects 중 하나**이다. Direct3D는 또 다른 COM objects를 내부에 가지고 있다. 여기에는 software, hardware, whatever-ware를 사용하여 2D, 3D 그래픽을 실행하는 데 필요한 모든 것이 포함된다.   
```cpp
device->CreateRenderTargetView();
device->Release();
```
위 코드는 **Direct3D가 가진 COM class( device )이고, 이러한 COM 객체를 멤버 함수로 제어**한다.   

## COM object에 대해 꼭 알아야 할 네 가지 사항
COM의 역할은 이것이 수행하는 복잡한 로직을 숨기는 것이지만, **꼭 알아야 할 네 가지 사항**이 있다.   
```
1. COM objects는 interface를 제어하기 위한 class 또는 set of classes이다. interface는 COM object를 제어하기 위한 function을 가진다. 예를 들면, device는 COM object이고, member function은 이러한 device를 제어한다.

2. 각 COM objects는 unique ID를 가진다. Direct3D는 자신만의 ID를 가지고 있으며, DirectSound object 또한 마찬가지다. 때때로 이러한 ID가 필요하다.

3. 하나의 COM object를 다 사용했으면, 항상 Release() 함수를 호출해야만 한다. 이 함수는 COM object에게 memory와 자신을 담당했던 threads를 해제하도록 명령한다.

4. Direct3D에서 제공하는 COM object는 쉽게 알아차릴 수 있다. "ID3D11Device"처럼 이름 맨 앞에 "I"가 존재하기 때문이다. 
```



# ComPtr
`ComPtr`은 COM object에 대해 C++의 smart pointer처럼 동작하는 template이다.   

## 1. Initialize
```cpp
#include <wrl/client.h>   // ComPtr

using Microsoft::WRL::Comptr;
// Declaration
Microsoft::WRL::ComPtr<ID3D11Device> device;          // COM interface
Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

// C++ smart pointer( Declaration and Initialization )
std::shared_ptr<ID3D11Device> device = make_shared<ID3D11Device>(...);
ID3D11Device* device = nullptr;
```
COM interface를 초기화 하는 방식은 `용도에 맞는 함수를 지정하는 것`이다.   
new나 make()를 이용하지 않는다.   
```cpp
HRESULT hr = D3D11CreateDevice(
  nullptr,
  D3D_DRIVER_TYPE_HARDWARE,
  0,
  creationFlags,
  featureLevels,
  ARRAYSIZE(featureLevels),
  D3D11_SDK_VERSION,
  &device,                // Initialization
  &m_d3dFeatureLevel,
  &context
);
```

## 2. ETC
### FAILED() macro
```cpp
if (FAILED(hr)) {
  cout << "Failed." << endl;
  return -1;
}
```
HRESULT가 있는지 확인하고, 없으면 프로그램을 종료한다.   

### As() | Get(), GetAddressOf()
```cpp
ComPtr<ID3D11Device> m_d3dDevice;
hr = device.As(&m_d3dDevice);       // assignment

auto temp = m_d3dDevice.Get();      // &m_d3dDevice
```
Pointer 할당과 가져오기를 담당한다.   
GetAddressOf()는 이중 포인터를 가져온다. 예를 들면, 포인터를 저장하는 배열이 있다.   
```cpp
ID3D11Buffer* pptr[1] = {
  m_constantBuffer.Get(),
};
m_context->VSSetConstantBuffers(0, 1, pptr);    // pptr == m_constantBuffer.GetAddressOf()
```
이처럼 주소 연산자( address-of, &: ampersand ) 보다 Get(), GetAddressOf()를 명확하게 사용하는 것을 권장한다.   
operator&은 ReleaseAndGetAddressOf()이기 때문( [DirectXTK_ComPtr - github](https://github.com/Microsoft/DirectXTK/wiki/ComPtr#initialization) )   


### Reset()
```cpp
m_d3dDevice.Reset();    // nullptr
```
Com Object를 Release 한다.   