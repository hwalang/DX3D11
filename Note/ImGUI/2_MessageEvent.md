- [Introduce](#introduce)
- [ImGUI 메시지 처리](#imgui-메시지-처리)
- [Main Loop](#main-loop)
  - [1. Frame Cycle](#1-frame-cycle)
  - [2. ChildApp::UpdateGUI()](#2-childappupdategui)

# Introduce
MainLoop에서 ImGUI를 사용하는 방법을 알아본다.   
매 프레임마다 GUI 이벤트를 처리하기 위해 필요하다.   

# ImGUI 메시지 처리
```cpp
// imgui_impl_win32.cpp에 정의된 메시지 처리 함수에 대한 전방 선언
// 해당 코드가 없으면 함수를 사용할 수 없다.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler ( 
	HWND hWnd , UINT msg , WPARAM wParam ,LPARAM lParam );

// ...

LRESULT CALLBACK AppBase::MsgProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam ) {
	// ImGUI의 사용자 이벤트를 처리
	if ( ImGui_ImplWin32_WndProcHandler ( hwnd , msg , wParam , lParam ) ) {
		return true;
	}

	// ...
}
```
**ImGUI는 `ImGui_ImplWin32_WndProcHandler()`를 이용하여 사용자 이벤트 메시지를 처리**할 수 있다.   
GUI가 이벤트를 인식하여 다양한 작업을 할 수 있도록 한다.   

# Main Loop
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

			ImGui::Text ( "Average %.3f ms/frame (%.1f FPS)" , 1000.0f / ImGui::GetIO().Framerate , ImGui::GetIO ().Framerate );
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

## 1. Frame Cycle
```cpp
ImGui_ImplDX11_NewFrame(); // GUI 프레임 시작
ImGui_ImplWin32_NewFrame();

ImGui::NewFrame(); // 어떤 것들을 렌더링 할지 기록 시작
ImGui::Begin("Scene Control");

// 매 프레임마다 변화하는 GUI 기능을 추가

ImGui::End ();
ImGui::Render (); // Rendering 대상 기록 끝
ImGui_ImplDX11_RenderDrawData ( ImGui::GetDrawData () ); // GUI Rendering
```
`DeltaTime`은 이전 프레임에서 현재 프레임까지 시간 차이를 알려준다.   

## 2. ChildApp::UpdateGUI()
자식 Apps에서 각자만의 GUI를 구현한다.   
```cpp
ImGui::Begin("Scene Control");

UpdateGUI(); // 추가적으로 사용할 GUI

ImGui::End();
```
`UpdateGUI()`는 Begin과 End 사이에 Framerate를 출력하는 부분 이외의 GUI를 Rendering 한다.   
```cpp
void CubeApp::UpdateGUI () {
  ImGui::Checkbox ( "usePerspectiveProjection" , &m_usePerspectiveProjection );
}
```
예를 들면, `CubeApp`은 perspective projection 또는 orthographic projection을 사용할 것인지 선택하는 체크 박스 GUI를 생성한다.   