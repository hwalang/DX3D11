[MSLearn - ComPtr Class](https://learn.microsoft.com/en-us/cpp/cppcx/wrl/comptr-class?view=msvc-170)   
[GitHub - ComPtr ](https://github.com/Microsoft/DirectXTK/wiki/ComPtr#initialization)   

# Component Object Model( COM )
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