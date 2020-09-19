#include <src/internal/gresslib_internal.h>

#include <stdlib.h>

GRESSLIB_Window* GRESSLIB_AllocateWindow(GRESSLIB_WindowDescriptor* const window_descriptor)
{
	//allocate the window struct
    void* window_alloc = malloc(sizeof(GRESSLIB_Window));

	if (window_alloc == NULL)
		return NULL;

	GRESSLIB_Window* wnd = (GRESSLIB_Window*)window_alloc;
    wnd->descriptor = *window_descriptor;

	wnd->onKeyPress = wnd->onKeyRelease = wnd->onMouseButtonPress =
	wnd->onMouseButtonRelease = wnd->onMouseMove = wnd->onMouseWheelMove = NULL;

    return wnd;
}

void GRESSLIB_SetInputEventCallback(GRESSLIB_Window* const window, const enum GRESSLIB_InputEventType event, GRESSLIB_InputEventCallback callback)
{
	//set the event callback
	switch (event)
	{
	default:
		break;
	case KEY_PRESS:
		window->onKeyPress = callback;
		break;
	case KEY_RELEASE:
		window->onKeyRelease = callback;
		break;
	case MOUSE_MOVE:
		window->onMouseMove = callback;
		break;
	case MOUSEBUTTON_PRESS:
		window->onMouseButtonPress = callback;
		break;
	case MOUSEBUTTON_RELEASE:
		window->onMouseButtonRelease = callback;
		break;
	case MOUSEWHEEL_MOVE:
		window->onMouseWheelMove = callback;
		break;
	}
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
