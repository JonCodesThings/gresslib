#include <include/gresslib/gresslib.h>

#include <windows.h>

LRESULT CALLBACK Win32WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

enum keyboard_keycodes virtual_key_to_gresslib_keycode(USHORT vkey);

struct window* create_window(struct window_descriptor* const window_desc)
{
	LPCSTR name = "gresslib_win32_window_class";

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = Win32WindowProc;
	wc.hInstance = GetModuleHandleW(NULL);
	wc.lpszClassName = name;

	if (RegisterClass(&wc) == FALSE)
		return NULL;

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

	void* window_alloc = malloc(sizeof(struct window));

	if (window_alloc == NULL)
		return NULL;

	LPCSTR window_title = window_desc->title;

	unsigned int style;

	if (window_desc->style & WINDOW_BORDERLESS)
		style = WS_POPUP;
	else
		style = WS_OVERLAPPEDWINDOW;

	if (!(window_desc->style & WINDOW_RESIZEABLE))
		style = style&~WS_MAXIMIZEBOX;

	HWND win32_window = CreateWindowEx(0, name, window_title, style&~WS_SIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, window_desc->width, window_desc->height, NULL, NULL, wc.hInstance, NULL);

	if (win32_window == NULL)
		return NULL;

	struct window* window = (struct window*)window_alloc;

	window->descriptor = *window_desc;

	window->native_handle = win32_window;

	window->on_key_press = NULL;
	window->on_key_release = NULL;
	window->on_mouse_move = NULL;
	window->on_mouse_button_press = NULL;
	window->on_mouse_button_release = NULL;
	window->on_mouse_wheel_move = NULL;

	LPCSTR property_name = "gresslib_handle";
	SetProp(win32_window, property_name, window);

	property_name = "gresslib_rawinput_buffer";
	SetProp(win32_window, property_name, NULL);

	property_name = "gresslib_rawinput_buffer_size";
	unsigned int* size = malloc(sizeof(unsigned int));
	*size = 0;
	SetProp(win32_window, property_name, size);
	
	ShowWindow(win32_window, SW_SHOW);

	return window;
}

bool destroy_window(struct window* window)
{
	if (!window)
		return true;

	LPCSTR property_name = "gresslib_handle";
	RemoveProp(window->native_handle, property_name);

	property_name = "gresslib_rawinput_buffer";
	if ((LPBYTE*)GetProp(window->native_handle, property_name) != NULL)
		free((LPBYTE*)GetProp(window->native_handle, property_name));

	RemoveProp(window->native_handle, property_name);

	property_name = "gresslib_rawinput_buffer_size";
	RemoveProp(window->native_handle, property_name);
	SendMessage(window->native_handle, WM_QUIT, 0, 0);

	DestroyWindow(window->native_handle);

	free(window);
	window = NULL;

	return true;
}

bool process_os_events(struct window* const window)
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			return false;
	}
	return true;
}

void set_input_event_callback(struct window* const window, const enum input_events event, input_callback callback)
{
	switch (event)
	{
	case KEY_PRESS:
		window->on_key_press = callback;
		break;
	case KEY_RELEASE:
		window->on_key_release = callback;
		break;
	case MOUSE_MOVE:
		window->on_mouse_move = callback;
		break;
	case MOUSEBUTTON_PRESS:
		window->on_mouse_button_press = callback;
		break;
	case MOUSEBUTTON_RELEASE:
		window->on_mouse_button_release = callback;
		break;
	case MOUSEWHEEL_MOVE:
		window->on_mouse_wheel_move = callback;
		break;
	}
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
		unsigned int size;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

		LPCSTR property_name = "gresslib_rawinput_buffer_size";

		unsigned int stored_size = *(unsigned int*)GetProp(hwnd, property_name);

		if (stored_size < size)
		{
			property_name = "gresslib_rawinput_buffer";
			if ((LPBYTE*)GetProp(hwnd, property_name) != NULL)
				free((LPBYTE*)GetProp(hwnd, property_name));

			LPBYTE rawinput_buffer = malloc(size);
			if (!rawinput_buffer)
				return 1;

			SetProp(hwnd, property_name, rawinput_buffer);

			property_name = "gresslib_rawinput_buffer_size";
			unsigned int* sz = (unsigned int *)(GetProp(hwnd, property_name));
			*sz = size;
		}

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (RAWINPUT*)GetProp(hwnd, "gresslib_rawinput_buffer"), &size, sizeof(RAWINPUTHEADER));

		RAWINPUT* input = (RAWINPUT*)GetProp(hwnd, "gresslib_rawinput_buffer");
		if (!input)
			break;

		struct window* window = GetProp(hwnd, "gresslib_handle");

		struct input_event ev;

		switch (input->header.dwType)
		{
		case RIM_TYPEKEYBOARD:
		{
			ev.keycode = virtual_key_to_gresslib_keycode(input->data.keyboard.VKey);
			if (input->data.keyboard.Flags == RI_KEY_BREAK)
			{
				if (window->on_key_release)
				{
					ev.event_type = KEY_RELEASE;
					window->on_key_release(&ev);
				}

			}
			else if (input->data.keyboard.Flags == RI_KEY_MAKE)
			{
				if (window->on_key_press)
				{
					ev.event_type = KEY_PRESS;
					window->on_key_press(&ev);
				}

			}
			break;
		}
		case RIM_TYPEMOUSE:
			switch (input->data.mouse.usFlags)
			{
			default:
				break;
			case MOUSE_MOVE_RELATIVE:
			case MOUSE_MOVE_ABSOLUTE:
			{
				ev.event_type = MOUSE_MOVE;
				POINT pos;
				GetCursorPos(&pos);
				ScreenToClient(hwnd, &pos);
				ev.mouse_x = pos.x;
				ev.mouse_y = pos.y;
				if (window->on_mouse_move)
					window->on_mouse_move(&ev);
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
				ev.event_type = MOUSEBUTTON_PRESS;
				break;
			case RI_MOUSE_BUTTON_1_UP:
			case RI_MOUSE_BUTTON_2_UP:
			case RI_MOUSE_BUTTON_3_UP:
			case RI_MOUSE_BUTTON_4_UP:
			case RI_MOUSE_BUTTON_5_UP:
				ev.event_type = MOUSEBUTTON_RELEASE;
				break;
			case RI_MOUSE_WHEEL:
				ev.event_type = MOUSEWHEEL_MOVE;
				ev.mouse_wheel_delta = input->data.mouse.usButtonData;
				if (window->on_mouse_wheel_move)
					window->on_mouse_wheel_move(&ev);
				break;
			}

			switch (input->data.mouse.usButtonFlags)
			{
			default:
				break;
			case RI_MOUSE_BUTTON_1_DOWN:
			case RI_MOUSE_BUTTON_1_UP:
				ev.mouse_button = 1;
				break;
			case RI_MOUSE_BUTTON_2_DOWN:
			case RI_MOUSE_BUTTON_2_UP:
				ev.mouse_button = 2;
				break;
			case RI_MOUSE_BUTTON_3_DOWN:
			case RI_MOUSE_BUTTON_3_UP:
				ev.mouse_button = 3;
				break;
			case RI_MOUSE_BUTTON_4_DOWN:
			case RI_MOUSE_BUTTON_4_UP:
				ev.mouse_button = 4;
				break;
			case RI_MOUSE_BUTTON_5_DOWN:
			case RI_MOUSE_BUTTON_5_UP:
				ev.mouse_button = 5;
				break;
			}
			break;
		default:
			break;
		}
		break;
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

enum keyboard_keycodes virtual_key_to_gresslib_keycode(USHORT vkey)
{
	switch (vkey)
	{
	default:
		return UNDEFINED;
	case VK_BACK:
		return BACKSPACE;
	case VK_TAB:
		return TAB;
	case VK_RETURN:
		return ENTER;
	case VK_SHIFT:
		return LEFT_SHIFT;
	case VK_CONTROL:
		return LEFT_CONTROL;
	case VK_MENU:
		return LEFT_ALT;
	case VK_CAPITAL:
		return CAPS_LOCK;
	case VK_ESCAPE:
		return ESCAPE;
	case VK_SPACE:
		return SPACEBAR;
	case '0':
		return NUM_ZERO;
	case '1':
		return NUM_ONE;
	case '2':
		return NUM_TWO;
	case '3':
		return NUM_THREE;
	case '4':
		return NUM_FOUR;
	case '5':
		return NUM_FIVE;
	case '6':
		return NUM_SIX;
	case '7':
		return NUM_SEVEN;
	case '8':
		return NUM_EIGHT;
	case '9':
		return NUM_NINE;
	case 'Q':
		return Q;
	case 'W':
		return W;
	case 'E':
		return E;
	case 'R':
		return R;
	case 'T':
		return T;
	case 'Y':
		return Y;
	case 'U':
		return U;
	case 'I':
		return I;
	case 'O':
		return O;
	case 'P':
		return P;
	case 'A':
		return A;
	case 'S':
		return S;
	case 'D':
		return D;
	case 'F':
		return F;
	case 'G':
		return G;
	case 'H':
		return H;
	case 'J':
		return J;
	case 'K':
		return K;
	case 'L':
		return L;
	case 'Z':
		return Z;
	case 'X':
		return X;
	case 'C':
		return C;
	case 'V':
		return V;
	case 'B':
		return B;
	case 'N':
		return N;
	case 'M':
		return M;
	}
}
