#include <include/gresslib/gresslib.h>

void handle_input(input_event * ev)
{
	switch (ev->event_type)
	{
	case KEY_PRESS:
	{
		break;
	}
	}
}

int main()
{
	window_descriptor desc;
	desc.width = 640;
	desc.height = 480;
	desc.style = WINDOW_BORDERED;
	desc.title = "basic example";

	window *wnd = create_window(&desc);

	set_input_event_callback(wnd, KEY_PRESS, handle_input);

	glcontext_descriptor cd;
	cd.alpha_size = cd.blue_size = cd.green_size = cd.red_size = 8;
	cd.depth_size = 24;
	cd.stencil_size = 8;

	bootstrap_gl(wnd, &cd);

	while (process_os_events(wnd))
	{
		;
	}

	destroy_window(wnd);

	return 0;
}