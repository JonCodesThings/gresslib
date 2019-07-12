#include <include/gresslib/gresslib.h>
#include <src/internal/gresslib_internal.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>

struct x11_native_handle
{
    Display* display;
    Window window;
};

struct window* create_window(struct window_descriptor* const window_desc)
{
    Display* display = XOpenDisplay(0);

    if (!display)
        return NULL;

    int black_colour = BlackPixel(display, DefaultScreen(display));
    int white_colour = WhitePixel(display, DefaultScreen(display));

    XSetWindowAttributes attribs;
    attribs.background_pixel = black_colour;
    attribs.border_pixel = black_colour;

    Window window = XCreateWindow( display,XRootWindow(display, DefaultScreen(display)),
                            200, 200, 350, 200, 5, DefaultDepth(display, DefaultScreen(display)), InputOutput,
                            DefaultVisual(display, DefaultScreen(display)) ,CWBackPixel, &attribs);

    struct window* wnd = allocate_window(window_desc);

    Atom delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &delete_window, 1);

    wnd->native_handle = NULL;

    struct x11_native_handle* native_handle = malloc(sizeof(struct x11_native_handle));
    native_handle->display = display;
    native_handle->window = window;

    wnd->native_handle = (void*)native_handle;

    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);

    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, 0);

    XSetForeground(display, gc, white_colour);

    XStoreName(display, window, window_desc->title);

    XFlush(display);

    return wnd;
}

bool destroy_window(struct window* window)
{
    struct x11_native_handle* native_handle = (struct x11_native_handle*)window->native_handle;
    XCloseDisplay(native_handle->display);
    window->native_handle = NULL;
    return true;
}

bool process_os_events(struct window* const window)
{
    struct x11_native_handle* native_handle = (struct x11_native_handle*)window->native_handle;
    XEvent ev;
    while (XPending(native_handle->display))
    {
        XNextEvent(native_handle->display, &ev);
        if (XFilterEvent(&ev, None))
            continue;
        if (ev.type == MapNotify)
            return true;
        if (ev.type == ClientMessage)
        {
            Atom delete_window = XInternAtom(native_handle->display, "WM_DELETE_WINDOW", False);
            if (ev.xclient.data.l[0] == delete_window)
                return false;
        }
    }
    //XFlush(native_handle->display);
	return true;
}