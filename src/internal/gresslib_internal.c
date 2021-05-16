#include <src/internal/gresslib_internal.h>

#include <stdlib.h>

GRESSLIB_Window* GRESSLIB_AllocateWindow(GRESSLIB_WindowDescriptor* const window_descriptor)
{
	//allocate the window struct
    void* window_alloc = GRESSLIB_Allocate(sizeof(GRESSLIB_Window));

	if (window_alloc == NULL)
		return NULL;

	GRESSLIB_Window* wnd = (GRESSLIB_Window*)window_alloc;
    wnd->descriptor = *window_descriptor;

	wnd->onKeyPress = wnd->onKeyRelease = wnd->onMouseButtonPress =
	wnd->onMouseButtonRelease = wnd->onMouseMove = wnd->onMouseWheelMove = NULL;

    return wnd;
}

void GRESSLIB_SetInputEventCallback(GRESSLIB_Window * const window, const enum GRESSLIB_InputEventType event, GRESSLIB_InputEventCallback callback)
{
	//set the event callback

	if (event & KEY_PRESS)
		window->onKeyPress = callback;

	if (event & KEY_RELEASE)
		window->onKeyRelease = callback;

	if (event & MOUSE_MOVE)
		window->onMouseMove = callback;

	if (event & MOUSEBUTTON_PRESS)
		window->onMouseButtonPress = callback;

	if (event & MOUSEBUTTON_RELEASE)
		window->onMouseButtonRelease = callback;

	if (event & MOUSEWHEEL_MOVE)
		window->onMouseWheelMove = callback;
}

void GRESSLIB_RunInputEventCallback(GRESSLIB_Window* const window, GRESSLIB_InputEvent* const event) 
{
	//find the correct callback if it exists and call it
	switch (event->eventType)
	{
	default: return;
	case KEY_PRESS:
		if (window->onKeyPress)
			window->onKeyPress(event);
		return;
	case KEY_RELEASE:
		if (window->onKeyRelease)
			window->onKeyRelease(event);
		return;
	case MOUSE_MOVE:
		if (window->onMouseMove)
			window->onMouseMove(event);
		return;
	case MOUSEBUTTON_PRESS:
		if (window->onMouseButtonPress)
			window->onMouseButtonPress(event);
		return;
	case MOUSEBUTTON_RELEASE:
		if (window->onMouseButtonRelease)
			window->onMouseButtonRelease(event);
		return;
	case MOUSEWHEEL_MOVE:
		if (window->onMouseWheelMove)
			window->onMouseWheelMove(event);
		return;
	}	
}
