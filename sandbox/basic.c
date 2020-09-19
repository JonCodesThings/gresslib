#include <include/gresslib/gresslib.h>

void handle_input(GRESSLIB_InputEvent * ev)
{
	switch (ev->eventType)
	{
	case KEY_PRESS:
	{
		break;
	}
	}
}

int main()
{
	GRESSLIB_WindowDescriptor desc;
	desc.width = 640;
	desc.height = 480;
	desc.style = WINDOW_BORDERED;
	desc.title = "basic example";

	GRESSLIB_Window *wnd = GRESSLIB_CreateWindow(&desc);

	GRESSLIB_SetInputEventCallback(wnd, KEY_PRESS, handle_input);

	GRESSLIB_GLContextDescriptor cd;
	cd.alphaSize = cd.blueSize = cd.greenSize = cd.redSize = 8;
	cd.depthSize = 24;
	cd.stencilSize = 8;

	GRESSLIB_BootstrapGL(wnd, &cd);

	while (GRESSLIB_ProcessOSEvents(wnd))
	{
		;
	}

	GRESSLIB_DestroyWindow(wnd);

	return 0;
}