- [Introduce](#introduce)
- [Window Size vs Client Size](#window-size-vs-client-size)
- [The `AdjustWindowRect()` Function](#the-adjustwindowrect-function)
- [Final Code](#final-code)

<br><br>

[Window Size and Client Size - DirectXTutorial](http://www.directxtutorial.com/Lesson.aspx?lessonid=11-1-4)   

# Introduce
window창의 크기가 아닌 DirectX를 그리는 창( Client )의 크기를 정확하게 설정하는 함수를 알아본다.   

# Window Size vs Client Size
`CreatingWindowEx()`에서 window size를 1280 x 960으로 저장했다.   
하지만 client size는 window의 일부이다.   
![alt text](Images/WindowSize_ClientSize/window_client_size.png)   
여기서 알 수 있듯이 window size는 border( 테두리 ) 가장자리에서 확장되는 반면, client size는 border 안쪽에서 확장된다.   

Rendering을 할 때는 client에서만 그리기 때문에 정확한 client size를 알아야 한다.   
그렇지 않으면 client area와 다른 크기를 가진 image를 그릴 때, client size와 맞추기 위해 image가 stretched( 늘어난 ) 또는 shrunk( 축소된 )되어 그려진다.   

# The `AdjustWindowRect()` Function
**client area는 application content를 표기할 수 있는 영역을 의미**한다.   
**window decorations는 title bar, border, scrollbar 등 client area 밖에 존재하며, 이러한 decorations가 표시될 때 client area에 영향**을 준다.   
**RECT 구조체는 window area를 나타내는 데 사용되는 struct**이다.   

Rather than setting the window size and then determining the client size, it is ideal to determine the client size in advance, and then calculate the appropriate window size.   
To do this we will use the function `AdjustWindowRect()` before creating the window.   
```cpp
typedef struct RECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
};

BOOL AdjustWindowRect( LPRECT lpRect, DWORD dwStyle, BOOL bMenu );

// ---------------------------------------------------------------------------

RECT wr = { 0, 0, 1280, 960 };    // set the size, but not the position
AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, FALSE );    // adjust the size

mainWindow = CreatingWindowEx( NULL, 
                               wc.lpszClassName,
                               L"window1 title",
                               WS_OVERLAPPEDWINDOW,
                               0,
                               0,
                               wr.right - wr.left,  // width of the window
                               wr.bottom - wr.top,  // height of the window
                               NULL,
                               NULL,
                               wc.hInstance,
                               NULL
);
```
`AdjustWindowRect()` 함수는 **지정한 client area를 목표 크기로 가정하고, 해당 window style에 맞추어 실제 window rectangle을 계산**한다.   
`AdjustWindowRect()` 호출 전에는 `wr`이 client area를 의미하지만, 호출 후의 `wr`은 client area를 만족하기 위해 필요한 실제 window size를 나타내는 `RECT` 구조체가 된다.   
즉, `RECT wr`은 client area 영역을 나타내다가 함수를 호출한 후에는 `wr`이 실제 window size를 의미한다.   
간단히 말하면, **원하는 client area와 window style을 고려해서 실제 window size를 계산**한다.   

**첫 번째 인자로 `RECT` struct의 주소**를 받는다. `RECT`는 원하는 client area가 저장됐다. 호출 후에는 client area를 고려한 window size를 `RECT`에 저장한다.   
**두 번째 인자는 window sytle**이다. window border의 크기를 결정하기 위해서 이 데이터를 사용한다. 이는 window decorations의 크기 반영을 위해 사용한다.   
**세 번째 인자는 menu를 사용할지 말지를 결정하는 BOOL 값**이다.   

# Final Code
```cpp
// Creating Window의 Final Code와 같음
...

int screenWidth = 1280;
int screenHeight = 960;
RECT wr = { 0, 0, screenWidth, screenHeight };
AdjustWindowRect ( &wr , WS_OVERLAPPEDWINDOW , FALSE );

HWND mainWindow = CreateWindowEx ( 
	NULL ,
	wc.lpszClassName ,     // name of the window class
	L"window1 title" ,     // title of the window
	WS_OVERLAPPEDWINDOW ,  // window style
	0 ,                    // x-position of the window
	0 ,                    // y-position of the window
	wr.right - wr.left ,   // width of the window
	wr.bottom - wr.top ,   // height of the window
	NULL ,                 // we have no parent window
	NULL ,                 // we aren't using menus
	wc.hInstance ,         // apllication handle
	NULL				   // used with multiple windows
);

// Creating Window의 Final Code와 같음
...
```