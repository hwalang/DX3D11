- [Introduce](#introduce)
- [Graphics Hardware \& DirectX3D](#graphics-hardware--directx3d)
- [DirectX Graphics Infrastructure( DXGI )](#directx-graphics-infrastructure-dxgi-)
- [The Swap Chain](#the-swap-chain)
  - [GPU가 화면에 image를 표시하고 업데이트 하는 과정](#gpu가-화면에-image를-표시하고-업데이트-하는-과정)
  - [problem - tearing](#problem---tearing)
  - [solution 1 - swapping](#solution-1---swapping)
  - [solution 2 - Address swapping( swap chain )](#solution-2---address-swapping-swap-chain-)
- [Rendering Pipeline](#rendering-pipeline)
- [The Coordinate Systems](#the-coordinate-systems)
  - [The Cartesian Coordinates( 데카르트 좌표계 )](#the-cartesian-coordinates-데카르트-좌표계-)
  - [3D Coordinates](#3d-coordinates)
- [3D Geometry](#3d-geometry)

<br><br>

[Understanding Graphics Concepts - DirectXTutorial](http://www.directxtutorial.com/Lesson.aspx?lessonid=11-4-1)   

# Introduce
DirectX program을 만들기 위해 필요한 Graphics Concepts에 대해 알아본다.   

# Graphics Hardware & DirectX3D
DirectX3D는 게임 플랫폼이 아니라 hardware interface이다. 즉, DirectX3D를 사용한다는 것은 Graphics hardware를 직접 조작하는 의미다.   
![alt text](Images/GraphicsConcepts/CPU_GPU.png)   
Graphics Programming에서 주로 다루는 hardware는 GPU 또는 graphics processing unit( 그래픽 처리 장치 )이다.   
CPU( central processing unit )는 계산을 수행하고 컴퓨터를 지휘한다. **GPU는 Graphics에 대한 계산을 수행하고 graphic 출력을 모니터로 전달**한다.   

![alt text](Images/GraphicsConcepts/CPU_GPUSeparateMemory.png)   
graphic programming은 별도의 processor가 있을 뿐 아니라 video memory에서 작동한다.   
**video memory는** motherboard에 존재하지 않고 video card에 존재하기 때문에 **빠르게 GPU에 접근**할 수 있다. 그리고 **화면에 현재 image와 다음 image를 compile 하는데 사용되는 모든 데이터를 저장**한다.   

**DirectX는 data가 system memory 또는 video memory에 저장되는 시기를 관리**한다.   

# DirectX Graphics Infrastructure( DXGI )
DXGI는 화면에 image를 보여주고, monitor와 video card가 처리할 수 있는 resolution( 해상도 )를 찾는 등 기본적인 작업을 처리하는 역할이다.   
![alt text](Images/GraphicsConcepts/DXGI.png)   
또한 **Direct3D와 hardware 간의 interface 역할을 수행**한다.   

DXGI를 직접 다루는 방법은 넘어가고, **이러한 component가 존재한다는 사실만 인지**한다. 왜냐하면 Direct3D에는 DXGI만 처리하는 부분이 있기 때문이다.   

# The Swap Chain
## GPU가 화면에 image를 표시하고 업데이트 하는 과정
- GPU는 자신의 memory에 현재 화면에 올라온 image의 픽셀 buffer에 대한 pointer를 가지고 있다. 이러한 pixel buffer는 화면에 표시될 image의 각 pixel data를 저장하는 memory space이다. 이러한 buffer에 손쉽게 접근하기 위해서 pointer를 저장한다.
- 3D model이나 image를 rendering할 때, GPU는 그것에 대한 pixel buffer를 수정하고 이 정보를 monitor에 보낸다.
- monitor는 수신한 pixel data를 바탕으로 top에서 bottom으로 화면을 다시 그려서 old image를 new image로 교체한다.

## problem - tearing
하지만 위 과정은 **real-time rendering에 필요한 만큼 monitor가 빠르게 새로고침되지 않는 점이 문제**다.   
만약 monitor가 새로고침 되는 동안 다른 model 또는 image가 GPU에 rendering 되면, **monitor에 표시되는 image가 두 개로 잘려서 위쪽에는 previous image가, 아래쪽에는 new image가 표시**된다.   
이러한 현상을 **tearing**이라 부른다.   

## solution 1 - swapping
tearing을 피하기 위해서 DXGI는 **swapping**이라 부르는 기능을 제공한다.   
DXGI는 new image를 monitor에 직접 rendering하는 대신에 **back buffer**라 불리는 secondary( 보조 ) pixel buffer에 image를 그린다. 반대로 **front buffer**는 현재 monitor에 그려지는 pixel buffer다.   
**DXGI는 back buffer에 image가 모두 그려지면, 이를 front buffer로 업데이트하고 previous image를 삭제**한다.   

## solution 2 - Address swapping( swap chain )
swapping을 통해 new image를 back buffer에 올리고, previous image를 front buffer에서 관리함에도 여전히 tearing이 발생할 수 있다.   
왜냐하면 GPU가 monitor로 image를 전송하는 속도가 monitor의 새로고침 속도보다 훨씬 빠르기 때문이다. 즉, monitor가 새로고침하는 동안 image transfer( 전송 )이 계속 발생할 수 있기 때문이다.   

![alt text](Images/GraphicsConcepts/AddressSwapInstantly.png)   
tearing을 피하기 위해서 더 빠른 속도가 필요하기 때문에 **DXGI는 각 buffer에 대한 pointer를 사용하여 간단하게 두 buffer의 값을 바꾼다**.   
![alt text](Images/GraphicsConcepts/MultipleBackBuffer.png)   
back buffer를 더 추가해서 게임의 성능을 더 좋게 만들 수 있다.   
이러한 방법을 **swap chain**이라 부르며, **new frame이 rendering 될 때마다 위치를 교체하는 buffer chain**이다.   

# Rendering Pipeline
![alt text](Images/GraphicsConcepts/RenderingPipeline.png)   
Rendering Pipeline은 화면에 rendered 3D image를 생성하는 process이다. Rendering Pipeline은 GPU에서 수행되며, 하나씩 처리해나간다.   
이해를 돕기 위한 핵심적인 단계만 살펴보고 넘어간다.   

**Input-Assembler Stage**는 pipeline의 첫 단계이다. Rendering 하려는 3D model에 대한 정보를 video memory에서 수집하고, 이러한 정보를 compile하고 rendering 할 수 있도록 준비한다.   

**Rasterizer Stage**는 back buffer의 image가 그려질 위치, 더 구체적으로 어떤 pixel이 그려지고 어떤 color로 그려지는지 결정하는 역할이다.   

**Output-Merger( 출력-병합 ) Stage**는 pipeline의 마지막 단계이다. 개별 model image를 하나의 image로 결합하고, 그 image를 back buffer에 배치하는 작업을 수행한다.   

# The Coordinate Systems

## The Cartesian Coordinates( 데카르트 좌표계 )
![alt text](Images/GraphicsConcepts/CartesianCooridnates.png)   
**2D coordinates**라고 부르는 것이 더 명확하다. 즉, **평평한 표면에서 정확한 point를 찾는 system**이다.   
horizontal axis( x-axis )는 1D Cooridnates에 속하며, origin( 0 )으로부터 얼만큼 이동했는지 distance를 나타낸다.   
vertical axis( y-axis )는 1D coordinates에서 다른 direction으로 이동할 수 있는 axis이다. 이 또한 origin( 0 )으로부터 얼만큼 이동했는지 나타낸다.   
즉, **각 axis는 origin을 가지고 이 origin에서 얼만큼 떨어졌는지 표현**한다.   
위 이미지는 어떠한 point가 origin으로부터 (12, 4)만큼 떨어진 위치에 있음을 나타낸다.   

## 3D Coordinates
2D Coordinates의 x, y-axis에 수직인 axis( z-axis )를 추가하면 3D Coordinates가 된다.   

# 3D Geometry
3D coordinates에서 한 point가 space 상의 한 지점을 나타낸다면, 우리는 3D model의 정확한 위치를 array로 구성할 수 있다.   
이러한 작업은 매우 많은 points를 memory 공간에 할당한다. 때문에 더 쉽고 더 빠르게 표현하는 방법이 triangles다.   
![alt text](Images/GraphicsConcepts/TrianglesModel.png)   
이처럼 triangles를 이용해서 다양한 polygon을 표현할 수 있기 때문이다.   
**DirectX는 삼각형과 삼각형이 결합하여 shapes를 만드는 데에만 초점을 맞춘다**. 이러한 삼각형을 만들기 위해서 vertices를 사용한다.   
