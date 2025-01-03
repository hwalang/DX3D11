# Technical Terms
Computer Graphics에서 사용하는 용어를 정리   

## 1. Buffer
일반적으로 `데이터를 임시로 저장하기 위해 사용되는 메모리 공간`이다.   
데이터 처리 효율성을 높이고, 데이터 손실을 방지하기 위한 핵심 개념이다.   

Graphics에서 Buffer라는 용어가 나오면 `일반적으로 Graphic Card의 memory를 의미`한다. 이는 Shdaer Programming에서 GPU 자원( ID3D11Buffer, ID3D11Resource )를 뜻한다.   
그 예로는 vertex, index, constant buffer가 존재한다.   
**shader와 program이 연동되는 변수**라고 생각하면 편하다.   

## 2. Projection
**3차원 세상을 2차원 이미지로 만드는 과정**   
viewing frustum, perspective or orthographic projection 등 3D를 2D 이미지로 변환하는 다양한 상황이 존재한다.   

## 3. Offset
**어떤 값을 기존 좌표에 더하거나 빼서 이동시키는 것을 의미**한다.   
```hlsl
return g_texture0.Sample(g_sampler, input.texcoord + float2(xSplit, 0.0));
```
해당 코드는 pixel shader에서 [texture tiling 효과](/Note/AppFramework/SquareApp.md/#13-texture-tiling)를 적용한다.   
여기서 `float2(xSplit, 0.0)`이 offset 역할을 수행한다.   
원래의 UV 좌표인 `input.texcoord`에 `xSplit` 값을 더해서 texture 좌표가 x축 방향으로 `xSplit`만큼 이동(shift, translate)되도록 하기 때문이다.   