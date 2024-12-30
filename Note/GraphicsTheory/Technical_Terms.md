# Technical Terms
Computer Graphics에서 사용하는 용어를 정리   

## 1. Buffer
일반적으로 `데이터를 임시로 저장하기 위해 사용되는 메모리 공간`이다.   
데이터 처리 효율성을 높이고, 데이터 손실을 방지하기 위한 핵심 개념이다.   

Graphics에서 Buffer라는 용어가 나오면 `일반적으로 Graphic Card의 memory를 의미`한다. 이는 Shdaer Programming에서 GPU 자원( ID3D11Buffer, ID3D11Resource )를 뜻한다.   
그 예로는 vertex, index, constant buffer가 존재한다.   
**shader와 program이 연동되는 변수**라고 생각하면 편하다.   

