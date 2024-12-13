- [외부 HW의 에러를 파악하는 방법](#외부-hw의-에러를-파악하는-방법)
  - [1. 모듈이 초기화가 됐는지 확인](#1-모듈이-초기화가-됐는지-확인)
  - [2. 멤버 변수를 초기화 하기 전, 지역 변수로 HW 체크](#2-멤버-변수를-초기화-하기-전-지역-변수로-hw-체크)
- [Windows Programming: 부모 클래스에서 해당 인스턴스의 포인터를 전역 변수로 선언하는 이유](#windows-programming-부모-클래스에서-해당-인스턴스의-포인터를-전역-변수로-선언하는-이유)
- [방대한 프로그램 코드를 읽는 방법](#방대한-프로그램-코드를-읽는-방법)

<br><br>

# 외부 HW의 에러를 파악하는 방법
## 1. 모듈이 초기화가 됐는지 확인
API를 이용하여 외부 HW를 사용하는 경우 호환성, 기기 결함 등 다양한 문제로 인해 같은 코드를 작성하더라도 작동하지 않는 경우가 있다.   
이를 파악 또는 방지하기 위해서 특정 변수를 초기화하거나 기능을 수행하는 경우, 각 작업마다 확인을 거친다.   
```cpp
TestApp app;
app.Initialze();
```
일반적인 코드 작성법은 초기화할 때, 아무런 문제가 없기 때문에 따로 확인을 거치지 않는다.   
```cpp
int main() {
  if (!app.Initialize()) {
    cout << "Initialization Failed" << endl;
    return -1;
  }

  return app.Run();
}
```
하지만 다양한 그래픽 카드를 사용하는 DirectX 같은 경우, 각 단계마다 확인 작업이 필요하다.   
작업도 마찬가지다. `app.Run()`이 수행 중 비정상적으로 종료되면, Run()이 -1을 반환하여 문제가 있음을 알려준다.   

## 2. 멤버 변수를 초기화 하기 전, 지역 변수로 HW 체크
```cpp
// Init
ComPtr<ID3D11Device> device;
ComPtr<ID3D11DeviceContext> context;

const D3D_FEATURE_LEVEL featureLevels[2] = {
  D3D_FEATURE_LEVEL_11_0,       // 더 높은 버전이 앞에 위치
  D3D_FEATURE_LEVEL_9_3,
};
D3D_FEATURE_LEVEL featureLevel;

if (FAILED(D3D11CreateDevice(
  nullptr, driverType, 0, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels),
  D3D11_SDK_VERSION, &device, &featureLevel, &context
))) {
  cout << "D3D11CreateDevice() failed" << endl;
  return false;   // 프로그램 종료
}

// 멤버 변수로 복사
if (FAILED(device.As(&m_device))) {
  cout << "device.As() failed" << endl;
  return false;
}
if (FAILED(context.As(&m_context))) {
  cout << "context.As() failed" << endl;
  return false;
}
```
**m_device, m_context가 아닌 지역 변수로 먼저 device와 context**를 불러온다.   
D3D11을 지원하지 않는 HW를 사용하는 경우, 바로 멤버 변수에 초기화하면 어디서 에러가 발생했는지 알 수 없기 때문이다.   
따라서 멤버 변수에 저장하기 전 HW가 버전을 지원하는지 체크하고, 이를 멤버 변수에 붙여넣는다.   

즉, `사용자의 컴퓨터가 프로그램이 요구하는 버전을 지원하는지 체크하기 위해서 지역 변수를 이용`한다.   

만약 사용자의 컴퓨터가 프로그램이 요구하는 D3D11 버전이 아닌 경우, 버전을 낮춰서 D3D9 버전을 사용할 수 있도록 후처리를 해줄 수 있다.   

<br>

# Windows Programming: 부모 클래스에서 해당 인스턴스의 포인터를 전역 변수로 선언하는 이유
`Windows Programming에서 class의 멤버 함수는 callback 함수로 등록할 수 없다`.   
non-static 멤버 함수는 숨겨진 this pointer를 매개변수로 받기 때문에, `C 스타일의 callback 함수와 cignature와 맞지 않다`. 때문에 우회해서 이를 처리한다.   
```cpp
// Base.cpp
Base *g_base = nullptr;

// WNDCLASSEX 객체를 생성할 때 등록하는 callback 함수
LRESULT WINAPI WndProc(...) {
  return g_base->MsgProc(...);
}

Base::Base() : ... {
  g_base = this;
}

bool Base::Init() {
  WNDCLASSEX wc = {
    ...
    WndProc,    // callback 함수 등록
    ...
  };
}
```
클래스 내부에 멤버 함수가 아닌 일반적인 함수를 구현한 후, `함수 내부에서 자신(*this)에게 간접적으로 접근하여 class member function을 호출`한다.   
class member function에서 원하는 로직을 구현할 수 있으며, 이를 `상속받는 자식 클래스의 멤버 함수도 영향`을 받는다.   

이는 windows programming에서 메시지를 처리하기 위한 방법 중 하나다.   

<br>

# 방대한 프로그램 코드를 읽는 방법
```cpp
if (FAILED(D3D11CreateDevice(
        nullptr,                  // Specify nullptr to use the default adapter.
        driverType,               // Create a device using the hardware graphics driver.
        0,                        // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        createDeviceFlags,        // Set debug and Direct2D compatibility flags.
        featureLevels,            // List of feature levels this app can support.
        ARRAYSIZE(featureLevels), // Size of the list above.
        D3D11_SDK_VERSION, // Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
        &device,           // Returns the Direct3D device created.
        &featureLevel,     // Returns feature level of device created.
        &context           // Returns the device immediate context.
        ))) {
    cout << "D3D11CreateDevice() failed." << endl;
    return false;
}
```
다른 사람이 작성한 코드를 읽다가 많은 매개변수를 요구하는 함수를 만날 수 있다.   
이때 각 매개변수를 자세히 살펴보는 것이 아니라, `FAILED(CreateDevice())`에 초점을 맞춘다.   
"함수 실행이 실패했을 때, console에 log를 찍고 `false`를 반환하구나"로 알아차렸다면, 다음 코드로 넘어간다.   