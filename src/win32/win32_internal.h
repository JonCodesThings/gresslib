#ifndef GRESSLIB_WIN32_INTERNAL_H
#define GRESSLIB_WIN32_INTERNAL_H

#include <windows.h>

typedef struct
{
	HWND wnd;
	HDC hdc;
	HGLRC gl_context;
} win32_native_handle;

#endif