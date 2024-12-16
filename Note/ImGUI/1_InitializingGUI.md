# Introduce
ImGUI는 Immediate Mode GUI란 뜻이며, Graphics API를 이용해서 GUI component를 그리는 기능을 제공한다.   
때문에 매 프레임마다 화면 위에 GUI를 rendering 하는 방식이다.   

# Initialization
ImGUI의 Initialization에서는 ImGUI context를 생성하고 다양한 flags, fonts, styles를 세팅한다.   
그 후에 ImGUI의 환경( backend )를 초기화한다.   
```cpp
#include <imgui.h>					// main.lib
#include <imgui_impl_dx11.h>		// backend
#include <imgui_impl_win32.h>		// backend

bool InitGUI () {
	// setup imgui context
	IMGUI_CHECKVERSION ();
	ImGui::CreateContext ();
	ImGuiIO& io = ImGui::GetIO ();
	( void ) io;
	io.DisplaySize = ImVec2 ( float ( SCREEN_WIDTH ) , float ( SCREEN_HEIGHT ) );
	ImGui::StyleColorsLight ();

	// setup platform/renderer backends
	if ( !ImGui_ImplDX11_Init ( g_device , g_devcon )) {
		return false;
	}

	if ( !ImGui_ImplWin32_Init ( g_mainWindow ) ) {
		return false;
	}

	return true;
}
```
[Getting Started - IMGUI github](https://github.com/ocornut/imgui/wiki/Getting-Started#setting-up-dear-imgui--backends)   

## Backend?
ImGUI는 Windows OS와 상호작용이 가능한 platform backend가 존재하며, Graphics API( DX, OpenGL, ... )과 상호작용이 가능한 renderer backend가 존재한다.   
즉, 여기서 **backend는 다양한 platform과 graphic api에서 작동할 수 있도록 하는 interface 역할**을 나타낸다.   

이를 이용하면 동일한 코드를 사용하여 다양한 환경에서 GUI를 구현할 수 있다.   
compile 또는 run time에 platform과 renderer를 선택할 수 있도록 코드를 작성하면 가능하다.   