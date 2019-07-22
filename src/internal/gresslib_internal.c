#include <src/internal/gresslib_internal.h>

#include <stdlib.h>

struct window* allocate_window(struct window_descriptor* const window_descriptor)
{
    void* window_alloc = malloc(sizeof(struct window));

	if (window_alloc == NULL)
		return NULL;

    struct window* wnd = (struct window*)window_alloc;
    wnd->descriptor = *window_descriptor;

    return wnd;
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

void run_input_event_callback(struct window* const window, struct input_event* const event)
{
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