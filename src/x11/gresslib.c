#include <include/gresslib/gresslib.h>
#include <src/internal/gresslib_internal.h>
#include <src/x11/x11_internal.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

#include <stdlib.h>

enum keyboard_keycodes x_key_to_gresslib_key(KeySym keysym);

struct x11_native_handle
{
    Display* display;
    Window window;
    Atom delete_window;
    Atom window_hints;
};

//created using information from
//https://www.tonyobryan.com//index.php?article=9
struct _motif_wm_hints
{
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long InputMode;
    unsigned long status;
};


//constants lovingly ripped from SFML's Window implementation for X11
//https://github.com/SFML/SFML/blob/master/src/SFML/Window/Unix/WindowImplX11.cpp
const unsigned long _motif_wm_hints_functions = 1 << 0;
const unsigned long _motif_wm_hints_decorations = 1 << 1;

const unsigned long _motif_wm_hints_decoration_all = 1 << 0;
const unsigned long _motif_wm_hints_decoration_border = 1 << 1;
const unsigned long _motif_wm_hints_decoration_resize = 1 << 2;
const unsigned long _motif_wm_hints_decoration_title = 1 << 3;
const unsigned long _motif_wm_hints_decoration_menu = 1 << 4;
const unsigned long _motif_wm_hints_decoration_minimize = 1 << 5;
const unsigned long _motif_wm_hints_decoration_maximize = 1 << 6;

const unsigned long _motif_wm_hints_function_all = 1 << 0;
const unsigned long _motif_wm_hints_function_resize = 1 << 1;
const unsigned long _motif_wm_hints_function_move = 1 << 2;
const unsigned long _motif_wm_hints_function_minimize = 1 << 3;
const unsigned long _motif_wm_hints_function_maximize = 1 << 4;
const unsigned long _motif_wm_hints_function_close = 1 << 5;


struct window* create_window(struct window_descriptor* const window_desc)
{
    Display* display = XOpenDisplay(0);

    if (!display)
        return NULL;

    int black_colour = BlackPixel(display, DefaultScreen(display));
    int white_colour = WhitePixel(display, DefaultScreen(display));

    XSetWindowAttributes attribs;
    attribs.background_pixel = white_colour;
    attribs.border_pixel = black_colour;
    //attribs.override_redirect = true;

    XSizeHints* size = XAllocSizeHints();
    size->flags = PMinSize | PMaxSize;
    size->min_width = size->max_width = window_desc->width;
    size->min_height = size->max_height = window_desc->height;


    Window window = XCreateWindow(display, XRootWindow(display, DefaultScreen(display)),
                            200, 200, 350, 200, 5, DefaultDepth(display, DefaultScreen(display)), InputOutput,
                            DefaultVisual(display, DefaultScreen(display)) ,CWBackPixel, &attribs);

    XSetWMNormalHints(display, window, size);

    struct window* wnd = allocate_window(window_desc);

    wnd->native_handle = NULL;

    struct x11_native_handle* native_handle = malloc(sizeof(struct x11_native_handle));
    native_handle->display = display;
    native_handle->window = window;

    native_handle->delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &native_handle->delete_window, 1);

    native_handle->window_hints = XInternAtom(display, "_MOTIF_WM_HINTS", True);

    struct _motif_wm_hints window_hints;
    window_hints.flags = _motif_wm_hints_functions | _motif_wm_hints_decorations;
    window_hints.decorations = 0;
    window_hints.functions = 0;

    if (window_desc->style == WINDOW_BORDERED)
    {
//        window_hints.decorations = _motif_wm_hints_decoration_border | _motif_wm_hints_decoration_title  | _motif_wm_hints_decoration_minimize | _motif_wm_hints_decoration_menu;
        window_hints.decorations |= _motif_wm_hints_decoration_border | _motif_wm_hints_decoration_title | _motif_wm_hints_decoration_menu;
        window_hints.functions |=  _motif_wm_hints_function_close | _motif_wm_hints_decoration_resize;
    }


   int l =  XChangeProperty(display, window, native_handle->window_hints, native_handle->window_hints, 32, PropModeReplace, (unsigned char*)&window_hints, 5);

    wnd->native_handle = (void*)native_handle;

    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask);

    XMapWindow(display, window);

    XSetTransientForHint(display, window, RootWindow(display, DefaultScreen(display)));


    GC gc = XCreateGC(display, window, 0, 0);

    XSetForeground(display, gc, white_colour);

    XStoreName(display, window, window_desc->title);

    XFlush(display);

    XFree(size);

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
    XEvent e;
    struct input_event ev;
    while (XPending(native_handle->display))
    {
        ev.event_type = NONE;
        XNextEvent(native_handle->display, &e);
        switch (e.type)
        {
            default:
                return true;
            case ClientMessage:
            {
            if (e.xclient.data.l[0] == native_handle->delete_window)
                return false;
            }
            case KeyPress:
            {
                //KeySym sym = XkbKeycodeToKeysym(native_handle->display, e.xkey.keycode, 0, 0);
                //printf("%c\n", keysym2ucs(sym));
                ev.event_type = KEY_PRESS;
                ev.keycode = x_key_to_gresslib_key(XkbKeycodeToKeysym(native_handle->display, e.xkey.keycode, 0, 0));
                break;
            }
            case KeyRelease:
            {
                ev.event_type = KEY_RELEASE;
                ev.keycode = x_key_to_gresslib_key(XkbKeycodeToKeysym(native_handle->display, e.xkey.keycode, 0, 0));
                break;
            }
            case ButtonPress:
            {
                ev.event_type = MOUSEBUTTON_PRESS;
                switch (e.xbutton.button)
                {
                    default:
                        ev.mouse_button = -1;
                        break;
                    case 1:
                        ev.mouse_button = 1;
                        break;
                    case 2:
                        ev.mouse_button = 2;
                        break;
                    case 3:
                        ev.mouse_button = 3;
                        break;
                    case 4:
                        ev.mouse_button = 4;
                        break;
                    case 5:
                        ev.mouse_button = 5;
                        break;
                }
                break;
            }
            case ButtonRelease:
            {
                ev.event_type = MOUSEBUTTON_RELEASE;
                switch (e.xbutton.button)
                {
                    default:
                        ev.mouse_button = -1;
                        break;
                    case 1:
                        ev.mouse_button = 1;
                        break;
                    case 2:
                        ev.mouse_button = 2;
                        break;
                    case 3:
                        ev.mouse_button = 3;
                        break;
                    case 4:
                        ev.mouse_button = 4;
                        break;
                    case 5:
                        ev.mouse_button = 5;
                        break;
                }
                break;
            }
            case MotionNotify:
            {
                ev.event_type = MOUSE_MOVE;
                ev.mouse_x = e.xmotion.x;
                ev.mouse_y = e.xmotion.y;
                break;
            }
        }
        run_input_event_callback(window, &ev);
    }
	return true;
}

enum keyboard_keycodes x_key_to_gresslib_key(KeySym keysym)
{
    switch (keysym)
    {
    default:    return UNDEFINED;
    case XK_BackSpace:  return BACKSPACE;
	case XK_Tab:    return TAB;
	case XK_Return: return ENTER;
	case XK_Shift_L:  return LEFT_SHIFT;
	case XK_Control_L:  return LEFT_CONTROL;
	case XK_Alt_L:  return LEFT_ALT;
	case XK_Caps_Lock:  return CAPS_LOCK;
	case XK_Escape: return ESCAPE;
	case XK_space:  return SPACEBAR;
	case XK_0:   return NUM_ZERO;
	case XK_1:   return NUM_ONE;
	case XK_2:   return NUM_TWO;
	case XK_3:   return NUM_THREE;
	case XK_4:   return NUM_FOUR;
	case XK_5:   return NUM_FIVE;
	case XK_6:   return NUM_SIX;
	case XK_7:   return NUM_SEVEN;
	case XK_8:   return NUM_EIGHT;
	case XK_9:   return NUM_NINE;
	case XK_q:   return Q;
	case XK_w:   return W;
	case XK_e:   return E;
	case XK_r:   return R;
	case XK_t:   return T;
	case XK_y:   return Y;
	case XK_u:   return U;
	case XK_i:   return I;
	case XK_o:   return O;
	case XK_p:   return P;
	case XK_a:   return A;
	case XK_s:   return S;
	case XK_d:   return D;
	case XK_f:   return F;
	case XK_g:   return G;
	case XK_h:   return H;
	case XK_j:   return J;
	case XK_k:   return K;
	case XK_l:   return L;
	case XK_z:   return Z;
	case XK_x:   return X;
	case XK_c:   return C;
	case XK_v:   return V;
	case XK_b:   return B;
	case XK_n:   return N;
	case XK_m:   return M;
    }
}