#include <src/internal/gresslib_internal.h>

#include <stdlib.h>

window* allocate_window(window_descriptor* const window_descriptor)
{
	//allocate the window struct
    void* window_alloc = malloc(sizeof(window));

	if (window_alloc == NULL)
		return NULL;

    window* wnd = (window*)window_alloc;
    wnd->descriptor = *window_descriptor;

	wnd->on_key_press = wnd->on_key_release = wnd->on_mouse_button_press =
	wnd->on_mouse_button_release = wnd->on_mouse_move = wnd->on_mouse_wheel_move = NULL;

    return wnd;
}

void set_input_event_callback(window* const window, const enum input_events event, input_callback callback)
{
	//set the event callback
	switch (event)
	{
	default:
		break;
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

void run_input_event_callback(window* const window, input_event* const event)
{
	//find the correct callback if it exists and call it
	switch (event->event_type)
	{
	default: return;
	case KEY_PRESS:
		if (window->on_key_press)
			window->on_key_press(event);
		return;
	case KEY_RELEASE:
		if (window->on_key_release)
			window->on_key_release(event);
		return;
	case MOUSE_MOVE:
		if (window->on_mouse_move)
			window->on_mouse_move(event);
		return;
	case MOUSEBUTTON_PRESS:
		if (window->on_mouse_button_press)
			window->on_mouse_button_press(event);
		return;
	case MOUSEBUTTON_RELEASE:
		if (window->on_mouse_button_release)
			window->on_mouse_button_release(event);
		return;
	case MOUSEWHEEL_MOVE:
		if (window->on_mouse_wheel_move)
			window->on_mouse_wheel_move(event);
		return;
	}	
}
