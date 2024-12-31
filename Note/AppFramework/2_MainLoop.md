# AppBase::Run()
자식 App에서 상속을 기대하지 않고 만든 함수이며, 이는 곧 **자식 Apps는 해당 구조에 맞춰서 실행된다**는 의미다.   
```cpp
int AppBase::Run () {
	MSG msg = { 0 };
	while ( WM_QUIT != msg.message ) {
		if ( PeekMessage ( &msg , NULL , 0 , 0 , PM_REMOVE ) ) {
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
		}
		else {
			// GUI frame 시작
			ImGui_ImplDX11_NewFrame ();
			ImGui_ImplWin32_NewFrame ();
			ImGui::NewFrame (); // Rendering 대상 기록 시작
			ImGui::Begin ("Scene Control");

			ImGui::Text ( "Average %.3f ms/frame (%.1f FPS)" , 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO ().Framerate );
			UpdateGUI ();

			ImGui::End ();
			ImGui::Render (); // Rendering 대상 기록 끝

			Update ( ImGui::GetIO ().DeltaTime ); // 매 프레임에 어떻게 움직이나?
			Render (); // Rendering
			ImGui_ImplDX11_RenderDrawData ( ImGui::GetDrawData () ); // GUI Rendering

			// switch the back buffer and the front buffer
			m_swapChain->Present (1, 0);
		}
	}
	return 0;
}
```

## 1. Message
window 창에서 발생하는 사용자 이벤트( 마우스 클릭, 키보드 입력 등 )를 인지하여 수행한다.   

## 2. Rendering
Message가 없는 경우, 매 프레임마다 다음과 같은 과정을 거쳐서 화면에 Rendering 한다.   
`UpdateGUI()`, `Update()`, `Render()`는 직접 구현한다.   

### 2.1. ImGUI
[MessageEvent.md](/Note/ImGUI/2_MessageEvent.md)에서 자세히 설명한다.   
`Begin()`과 `End()` 사이에 프레임을 기록하는 GUI와 perspective projection과 orthographic projection 중 선택하는 checkbox GUI를 매 프레임마다 그린다.   
이때 **가상 세계의 model 들의 rendering이 끝나고, GUI rendering을 덮어 씌우는 형식으로 순서를 지킨다**.   