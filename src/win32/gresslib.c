#include <include/gresslib/gresslib.h>
#include <src/internal/gresslib_internal.h>
#include <src/win32/win32_internal.h>
#include <src/WGL/glbootstrap.h>

#include <windows.h>

GRESSLIB_ALLOC GRESSLIB_Allocate = &malloc;
GRESSLIB_DEALLOC GRESSLIB_Deallocate = &free;

static LPWSTR GRESSLIB_WindowClass = L"gresslib_Win32_WindowClass";
static LPCWSTR GRESSLIB_HandlePropertyName = L"gresslib_handle";
static LPCWSTR GRESSLIB_RawInputBufferPropertyName = L"gresslib_rawinput_buffer";
static LPCWSTR GRESSLIB_RawInputBufferSizePropertyName = L"gresslib_rawinput_buffersize";


void GRESSLIB_SetAllocator(GRESSLIB_ALLOC const alloc, GRESSLIB_DEALLOC const dealloc)
{
	GRESSLIB_Allocate = alloc;
	GRESSLIB_Deallocate = dealloc;
}

LRESULT CALLBACK Win32WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

enum GRESSLIB_KeyboardKeycode virtual_key_to_gresslib_keycode(USHORT vkey);

GRESSLIB_Window* GRESSLIB_CreateWindow(GRESSLIB_WindowDescriptor* const window_desc)
{
	RECT r = {0, 0, window_desc->width, window_desc->height};

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = Win32WindowProc;
	wc.hInstance = GetModuleHandleW(NULL);
	wc.lpszClassName = GRESSLIB_WindowClass;

	if (RegisterClass(&wc) == FALSE)
		return NULL;

	LPWSTR wndTitle = GRESSLIB_WindowClass;
	if (window_desc->title != NULL)
	{
		size_t titleLen = strlen(window_desc->title) + 1;
		wndTitle = GRESSLIB_Allocate(sizeof(wchar_t) * titleLen);
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wndTitle, titleLen, window_desc->title, titleLen);
	}

	RAWINPUTDEVICE devices[2];

	//adds mouse listening
	devices[0].usUsagePage = 0x01;
	devices[0].usUsage = 0x02;
	devices[0].dwFlags = 0;
	devices[0].hwndTarget = 0;

	//adds keyboard listening
	devices[1].usUsagePage = 0x01;
	devices[1].usUsage = 0x06;
	devices[1].dwFlags = 0;
	devices[1].hwndTarget = 0;

	if (RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
		return NULL;

	unsigned int style;

	if (window_desc->style & WINDOW_BORDERLESS)
		style = WS_POPUP;
	else
		style = WS_OVERLAPPEDWINDOW;

	if (!(window_desc->style & WINDOW_RESIZEABLE))
		style = style&~WS_MAXIMIZEBOX;

	AdjustWindowRect(&r, style, FALSE);

	HWND win32_window = CreateWindowExW(0, GRESSLIB_WindowClass, wndTitle, style&~WS_SIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, NULL, NULL, wc.hInstance, NULL);

	if (win32_window == NULL)
		return NULL;

	GRESSLIB_Window* window = GRESSLIB_AllocateWindow(window_desc);

	window->descriptor = *window_desc;

	win32_native_handle *native_handle = GRESSLIB_Allocate(sizeof(win32_native_handle));
	native_handle->wnd = win32_window;

	window->nativeHandle = native_handle;

	window->onKeyPress = NULL;
	window->onKeyRelease = NULL;
	window->onMouseMove = NULL;
	window->onMouseButtonPress = NULL;
	window->onMouseButtonRelease = NULL;
	window->onMouseWheelMove = NULL;

	SetPropW(win32_window, GRESSLIB_HandlePropertyName, window);
	SetPropW(win32_window, GRESSLIB_RawInputBufferPropertyName, NULL);

	unsigned int* size = GRESSLIB_Allocate(sizeof(unsigned int));
	*size = 0;
	SetPropW(win32_window, GRESSLIB_RawInputBufferSizePropertyName, size);
	
	ShowWindow(win32_window, SW_SHOW);

	return window;
}

enum GRESSLIB_DestroyWindowResult GRESSLIB_DestroyWindow(GRESSLIB_Window* const window)
{
	if (!window)
		return GRESSLIB_DESTROYWINDOW_Failed;

	win32_native_handle *native_handle = window->nativeHandle;

	if ((LPBYTE*)GetProp(native_handle->wnd, GRESSLIB_RawInputBufferPropertyName) != NULL)
		GRESSLIB_Deallocate((LPBYTE*)GetProp(native_handle->wnd, GRESSLIB_RawInputBufferPropertyName));

	RemoveProp(native_handle->wnd, GRESSLIB_HandlePropertyName);
	RemoveProp(native_handle->wnd, GRESSLIB_RawInputBufferPropertyName);
	RemoveProp(native_handle->wnd, GRESSLIB_RawInputBufferSizePropertyName);

	SendMessage(native_handle->wnd, WM_QUIT, 0, 0);

	DestroyWindow(native_handle->wnd);

	GRESSLIB_Deallocate(window);

	return GRESSLIB_DESTROYWINDOW_Success;
}

enum GRESSLIB_ProcessOSEventsResult GRESSLIB_ProcessOSEvents(GRESSLIB_Window* const window)
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			return GRESSLIB_PROCESSOSEVENTS_QuitEvent;
	}
	return GRESSLIB_PROCESSOSEVENTS_NoQuitEvent;
}

LRESULT CALLBACK Win32WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_QUIT:
	{
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_INPUT:
	{
		unsigned int size = 0;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

		unsigned int storedSize = *(unsigned int*)GetPropW(hwnd, GRESSLIB_RawInputBufferSizePropertyName);

		if (storedSize < size)
		{
			if ((LPBYTE*)GetProp(hwnd, GRESSLIB_RawInputBufferPropertyName) != NULL)
				GRESSLIB_Deallocate((LPBYTE*)GetPropW(hwnd, GRESSLIB_RawInputBufferPropertyName));

			LPBYTE rawinput_buffer = GRESSLIB_Allocate(size);
			if (!rawinput_buffer)
				return 1;

			SetProp(hwnd, GRESSLIB_RawInputBufferPropertyName, rawinput_buffer);

			unsigned int* sz = (unsigned int *)(GetPropW(hwnd, GRESSLIB_RawInputBufferSizePropertyName));
			*sz = size;
		}

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (RAWINPUT*)GetPropW(hwnd, GRESSLIB_RawInputBufferPropertyName), &size, sizeof(RAWINPUTHEADER));

		RAWINPUT* input = (RAWINPUT*)GetPropW(hwnd, GRESSLIB_RawInputBufferPropertyName);
		if (!input)
			break;

		GRESSLIB_Window* window = GetPropW(hwnd, GRESSLIB_HandlePropertyName);

		GRESSLIB_InputEvent ev;

		switch (input->header.dwType)
		{
		case RIM_TYPEKEYBOARD:
		{
			ev.keycode = virtual_key_to_gresslib_keycode(input->data.keyboard.VKey);
			if (input->data.keyboard.Flags == RI_KEY_BREAK)
			{
				if (window->onKeyRelease)
				{
					ev.eventType = KEY_RELEASE;
					window->onKeyRelease(&ev);
				}

			}
			else if (input->data.keyboard.Flags == RI_KEY_MAKE)
			{
				if (window->onKeyPress)
				{
					ev.eventType = KEY_PRESS;
					window->onKeyPress(&ev);
				}

			}
			return TRUE;
		}
		case RIM_TYPEMOUSE:
			switch (input->data.mouse.usFlags)
			{
			default:
				break;
			case MOUSE_MOVE_RELATIVE:
			case MOUSE_MOVE_ABSOLUTE:
			{
				ev.eventType = MOUSE_MOVE;
				POINT pos;
				GetCursorPos(&pos);
				ScreenToClient(hwnd, &pos);
				ev.mouseX = pos.x;
				ev.mouseY = pos.y;
				if (window->onMouseMove)
					window->onMouseMove(&ev);
				break;
			}
			}

			switch (input->data.mouse.usButtonFlags)
			{
			default:
				break;
			case RI_MOUSE_BUTTON_1_DOWN:
			case RI_MOUSE_BUTTON_2_DOWN:
			case RI_MOUSE_BUTTON_3_DOWN:
			case RI_MOUSE_BUTTON_4_DOWN:
			case RI_MOUSE_BUTTON_5_DOWN:
				ev.eventType = MOUSEBUTTON_PRESS;
				break;
			case RI_MOUSE_BUTTON_1_UP:
			case RI_MOUSE_BUTTON_2_UP:
			case RI_MOUSE_BUTTON_3_UP:
			case RI_MOUSE_BUTTON_4_UP:
			case RI_MOUSE_BUTTON_5_UP:
				ev.eventType = MOUSEBUTTON_RELEASE;
				break;
			case RI_MOUSE_WHEEL:
				ev.eventType = MOUSEWHEEL_MOVE;
				ev.mouseWheelDelta = input->data.mouse.usButtonData;
				if (window->onMouseWheelMove)
					window->onMouseWheelMove(&ev);
				break;
			}

			switch (input->data.mouse.usButtonFlags)
			{
			default:
				break;
			case RI_MOUSE_BUTTON_1_DOWN:
			case RI_MOUSE_BUTTON_1_UP:
				ev.mouseButton = 1;
				break;
			case RI_MOUSE_BUTTON_2_DOWN:
			case RI_MOUSE_BUTTON_2_UP:
				ev.mouseButton = 2;
				break;
			case RI_MOUSE_BUTTON_3_DOWN:
			case RI_MOUSE_BUTTON_3_UP:
				ev.mouseButton = 3;
				break;
			case RI_MOUSE_BUTTON_4_DOWN:
			case RI_MOUSE_BUTTON_4_UP:
				ev.mouseButton = 4;
				break;
			case RI_MOUSE_BUTTON_5_DOWN:
			case RI_MOUSE_BUTTON_5_UP:
				ev.mouseButton = 5;
				break;
			}
			GRESSLIB_RunInputEventCallback(window, &ev);

			return TRUE;
		default:
			break;
		}
		break;
	}
	

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void GRESSLIB_ShowCursor(GRESSLIB_Window * const window)
{
	while (ShowCursor(TRUE) <= 0)
		ShowCursor(TRUE);
}

void GRESSLIB_HideCursor(GRESSLIB_Window * const window)
{
	while (ShowCursor(FALSE) >= 0)
		ShowCursor(FALSE);
}

void GRESSLIB_WarpCursor(GRESSLIB_Window * const window, const int x, const int y)
{
	win32_native_handle *native_handle = window->nativeHandle;
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(native_handle->wnd, &pt);
	SetCursorPos(pt.x, pt.y);
}


enum GRESSLIB_KeyboardKeycode virtual_key_to_gresslib_keycode(USHORT vkey)
{
	switch (vkey)
	{
	default:	return KEYCODE_UNDEFINED;
	case VK_BACK:	return BACKSPACE;
	case VK_TAB:	return TAB;
	case VK_RETURN:	return ENTER;
	case VK_SHIFT:	return LEFT_SHIFT;
	case VK_CONTROL:	return LEFT_CONTROL;
	case VK_MENU:	return LEFT_ALT;
	case VK_CAPITAL:	return CAPS_LOCK;
	case VK_ESCAPE:	return ESCAPE;
	case VK_SPACE:	return SPACEBAR;
	case '0':   return NUM_ZERO;
	case '1':   return NUM_ONE;
	case '2':   return NUM_TWO;
	case '3':   return NUM_THREE;
	case '4':   return NUM_FOUR;
	case '5':   return NUM_FIVE;
	case '6':   return NUM_SIX;
	case '7':   return NUM_SEVEN;
	case '8':   return NUM_EIGHT;
	case '9':   return NUM_NINE;
	case 'Q':   return Q;
	case 'W':   return W;
	case 'E':   return E;
	case 'R':   return R;
	case 'T':   return T;
	case 'Y':   return Y;
	case 'U':   return U;
	case 'I':   return I;
	case 'O':   return O;
	case 'P':   return P;
	case 'A':   return A;
	case 'S':   return S;
	case 'D':   return D;
	case 'F':   return F;
	case 'G':   return G;
	case 'H':   return H;
	case 'J':   return J;
	case 'K':   return K;
	case 'L':   return L;
	case 'Z':   return Z;
	case 'X':   return X;
	case 'C':   return C;
	case 'V':   return V;
	case 'B':   return B;
	case 'N':   return N;
	case 'M':   return M;
	}
}
