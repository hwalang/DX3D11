- [Introduce](#introduce)
- [Setting the Render Target](#setting-the-render-target)
  - [Render Target을 Setting하는 이유](#render-target을-setting하는-이유)


[Rendering Frames - DirectXTutorial](http://www.directxtutorial.com/Lesson.aspx?lessonid=11-4-3)   

# Introduce
Direct3D가 초기화되면 rendering을 시작한다. 따라서 rendering하기 위해 몇 가지 준비 작업이 필요하다.   
GPU가 최종 이미지를 생성할 memory 위치( back buffer )와 back buffer에 그릴 위치를 지정하는 작업이다.   

# Setting the Render Target
**rendering을 시작하는 곳을 우리는 back buffer로 알고 있지만, Direct3D는 알지 못한다**. 또한 **back buffer에 즉시 rendering 하는 것을 원치 않을 수 있다**.   
예를 들면, **많은 게임들은 model의 surface에 rendering 한 다음, 그 model을 back buffer에 rendering 한다**. 이러한 기술은 다양한 효과를 만들 수 있다. "포탈" 게임에서는 포탈에 먼저 rendering 한 다음, 포탈 이미지가 포함된 전체 장면을 rendering 한다.   
즉, **back buffer에 직접 rendering 하지 않고 중간 단계의 surface에 rendering 함으로써 다양한 graphic 효과를 구현하고, 최종적으로 완성된 이미지를 화면에 출력**한다.   

Direct3D에서 rendering을 하기 위해선 반드시 **render target을 지정**한다. 대부분의 경우 **이러한 render target은 back buffer를 의미**하고, 이는 rendering 할 video memory의 위치를 유지하는 간단한 **COM object**이다.   

즉, **video memory에 texture를 rendering하기 위해서 Render Target을 지정하는 방법**을 알아본다.   
```cpp
ID3D11RenderTargetView* backBuffer;

bool InitD3D( HWND hWnd ) {
  // Direct3D Initialization
  // ...

  // get the address of the back buffer
  ID3D11Texture2D* pBackBuffer;
  swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

  // use the back buffer address to create the render target
  dev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
  pBackBuffer->Release();

  // set the render target as the back buffer
  devcon->OMSetRenderTargets(1, &backBuffer, NULL);
}
```
위 코드는 **render target을 설정하는 3단계 과정**을 나타낸다.   
첫째, back buffer의 주소 얻기: `swapChain->GetBuffer()`를 사용해서 swap chain에서 back buffer의 texture를 가져온다.    
둘째, 그 주소를 사용해서 render target view를 나타내는 COM object를 생성한다. `dev->CreateRenderTargetView()`를 사용하여 back buffer texture를 기반으로 render target view를 생성한다.   
셋째, `pBackBuffer->Release()`를 호출하여 texture object를 해제한다.   
넷째, `devcon->OMSetRenderTargets()`를 사용해서 생성된 render target view를 현재 render target으로 설정한다.   

`ID3D11RenderTargetView*`는 **render target에 대한 모든 정보를 가진 object를 가리키는 pointer**다. back buffer에 rendering 할 것이기 때문에 해당 변수를 `backBuffer`로 부른다.   

**3D Rendering에서 texture의 또 다른 이름은 image이다**. `ID3D11Texture2D*`는 **image를 저장하는 COM object를 가리키는 pointer**다. 다른 COM object 처럼 먼저 pointer를 정의하고, 나중에 function으로 object를 생성한다.   

`swapChain->GetBuffer()` 는 **swap chain에서 back buffer를 찾고, `pBackBuffer` texture object를 생성하기 위해 사용**한다.   
**첫 번째 인자는 back buffer의 번호**를 나타낸다. 하나의 back buffer를 사용하기 때문에 `#0`으로 지정했다.   
**두 번째 인자는 `ID3D11Texture2D` COM object의 ID를 가져온다**. COM object의 각 TYPE은 자신만의 ID가 존재한다. 이러한 ID를 가져오기 위해서 `__uuidof` operator를 사용한다. 이를 통해 `GetBuffer()`가 어떤 TYPE의 COM object를 생성해야 하는지 알 수 있다.   
세 번째 인자를 설명하기 전, 기본 지식을 알아본다. **`void*` 포인터는 특정 TYPE의 변수를 가리키지 않는다. 또한 `void*`는 어떠한 type으로도 casting 될 수 있다**. 이처럼 **세 번째 인자는 `ID3D11Texture2D` Object의 주소를 가리키며**, 다른 type이 있을 수 있기에 `void*`을 사용한다.   
```cpp
#define IID_PPV_ARGS(ppType) __uuidof(**(ppType))

GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
```
참고로 `GetBuffer`의 2, 3번째 인자는 `IID_PPV_ARGS()`로 대체할 수 있다.   

**`dev->CreateRenderTargetView()` 함수는 render target object를 생성**한다. 생성된 객체를 가리키기 위해 `ID3D11RenderTargetView*`를 정의했다.   
다시 한 번 말하지만, COM object는 직접 다루는 것이 아니라 interface로 간접적으로 접근하여 다룬다.   
**첫 번째 인자는 `ID3D11Texture2D`를 가리키는 pointer**다.   
**두 번째 인자는 render target을 설명하는 struct**이다. back buffer는 기본 설정을 사용하기 때문에 `NULL`로 설정한다.   
**세 번째 인자는 생성된 render target view object를 받을 pointer 주소를 지정**한다.   

`devcon->OMSetRenderTargets()`는 여러 개의 render target을 지정한다.   
첫 번째 인자는 render target의 수를 나타낸다.   
두 번째 인자는 render target view object의 list를 가리키는 pointer다. 하나만 가지기 때문에 render target object의 주소만 넘겨줬다.   
세 번째 인자는 `... 나중에 알아봄!`   

## Render Target을 Setting하는 이유
이 작업을 통해 **back buffer를 render target으로 설정하여, 우리가 그리는 모든 것이 back buffer에 rendering 되고, 나중에 화면에 표시**될 수 있게 한다.   