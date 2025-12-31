#include <include/gresslib/gresslib.h>
#include <src/internal/gresslib_internal.h>
#include <src/x11/x11_internal.h>
#include <src/glx/glbootstrap.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>

#include <stdlib.h>

#include <stdio.h>

enum GRESSLIB_KeyboardKeycode x_key_to_gresslib_key(KeySym keysym);

//created using information from
//https://www.tonyobryan.com//index.php?article=9
//author: Tony O'Bryan
typedef struct
{
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long InputMode;
    unsigned long status;
} _motif_wm_hints;


//constants lovingly ripped from SFML's Window implementation for X11
//https://github.com/SFML/SFML/blob/master/src/SFML/Window/Unix/WindowImplX11.cpp
//author: SFML team
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

GRESSLIB_ALLOC GRESSLIB_Allocate = &malloc;
GRESSLIB_DEALLOC GRESSLIB_Deallocate = &free;

void GRESSLIB_SetAllocator(GRESSLIB_ALLOC const alloc, GRESSLIB_DEALLOC const dealloc)
{
    GRESSLIB_Allocate = alloc;
    GRESSLIB_Deallocate = dealloc;
}


GRESSLIB_Window * GRESSLIB_CreateWindow(GRESSLIB_WindowDescriptor* const window_desc)
{
    //open a connection to the display
    Display* display = XOpenDisplay(0);

    if (!display)
        return NULL;

    //get colour values
    int black_colour = BlackPixel(display, DefaultScreen(display));
    int white_colour = WhitePixel(display, DefaultScreen(display));

    //window attributes structure
    XSetWindowAttributes attribs;
    attribs.background_pixel = white_colour;
    attribs.border_pixel = black_colour;
    //attribs.override_redirect = true;

    //allocate size hints (prevents resizing)
    XSizeHints* size = XAllocSizeHints();
    size->flags = PMinSize | PMaxSize;
    size->min_width = size->max_width = window_desc->width;
    size->min_height = size->max_height = window_desc->height;

    //create the actual window
    Window w = XCreateWindow(display, XRootWindow(display, DefaultScreen(display)),
                            200, 200, 350, 200, 5, DefaultDepth(display, DefaultScreen(display)), InputOutput,
                            DefaultVisual(display, DefaultScreen(display)) ,CWBackPixel, &attribs);

    //enforce the size hints
    XSetWMNormalHints(display, w, size);

    //free the size hints from memory
    XFree(size);

    //allocate a new window struct
    GRESSLIB_Window *wnd = GRESSLIB_AllocateWindow(window_desc);

    wnd->nativeHandle = NULL;

    //manually allocate a native handle struct and set some members
    x11_native_handle* native_handle = GRESSLIB_Allocate(sizeof(x11_native_handle));
    native_handle->display = display;
    native_handle->window = w;
    native_handle->gl_context = NULL;

    //set up closing the window using the (x) button
    native_handle->delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, w, &native_handle->delete_window, 1);

    //get the motif window hints
    //this is basically legacy related code
    native_handle->window_hints = XInternAtom(display, "_MOTIF_WM_HINTS", True);

    //specify the hits we're changing
    _motif_wm_hints window_hints;
    window_hints.flags = _motif_wm_hints_functions | _motif_wm_hints_decorations;
    window_hints.decorations = 0;
    window_hints.functions = 0;

    if (native_handle->window_hints != None)
    {
	if (window_desc->style & WINDOW_BORDERED)
    	{
        	//window_hints.decorations = _motif_wm_hints_decoration_border | _motif_wm_hints_decoration_title  | _motif_wm_hints_decoration_minimize | _motif_wm_hints_decoration_menu;
        	window_hints.decorations |= _motif_wm_hints_decoration_border | _motif_wm_hints_decoration_title | _motif_wm_hints_decoration_menu;
        	window_hints.functions |=  _motif_wm_hints_function_close | _motif_wm_hints_decoration_resize;

		//change the property based on the hints
    		XChangeProperty(display, w, native_handle->window_hints, native_handle->window_hints, 32, PropModeReplace, (unsigned char*)&window_hints, 5);    
	}
    }
       
    //set the default cursor
    native_handle->normal = XCreateFontCursor(display, XC_left_ptr);

    //defines the cursor as the default
    XDefineCursor(native_handle->display, native_handle->window, native_handle->normal);

    //create a blank bitmap to use for the transparent cursor
    char raw_cursor_data;
    XColor colour;
    colour.red = colour.green = colour.blue = 0;
    Pixmap hidden_cursor_pixmap = XCreateBitmapFromData(native_handle->display, DefaultRootWindow(display), &raw_cursor_data, 1, 1);

    //create a transparent cursor
    native_handle->hidden = XCreatePixmapCursor(native_handle->display, hidden_cursor_pixmap, hidden_cursor_pixmap, &colour, &colour, 0, 0);

    //free the bitmap from memory after use
    XFreePixmap(native_handle->display, hidden_cursor_pixmap);

    //adding the native handle pointer
    wnd->nativeHandle = (void*)native_handle;

    //set which os events we want to receive
    XSelectInput(display, w, ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask);

    //actually show the window
    XMapWindow(display, w);

    //XSetTransientForHint(display, window, RootWindow(display, DefaultScreen(display)));

    //create a GC
    GC gc = XCreateGC(display, w, 0, 0);

    //set the default colour of the window to white
    XSetForeground(display, gc, white_colour);

    //set the window title
    XStoreName(display, w, window_desc->title);

    //send all those commands to the display server
    XFlush(display);

    return wnd;
}

enum GRESSLIB_DestroyWindowResult GRESSLIB_DestroyWindow(GRESSLIB_Window * window)
{
    x11_native_handle* native = (x11_native_handle*)window->nativeHandle;

    //destroy the gl context as required
    if (native->gl_context)
        GRESSLIB_ShutdownGL(window);
    
    //free both cursor resources
    XFreeCursor(native->display, native->normal);
    XFreeCursor(native->display, native->hidden);

    //close the display and free the native handle
    XCloseDisplay(native->display);
    GRESSLIB_Deallocate(native);

    window->nativeHandle = NULL;

    return GRESSLIB_DESTROYWINDOW_Success;
}

enum GRESSLIB_ProcessOSEventsResult GRESSLIB_ProcessOSEvents(GRESSLIB_Window * const window)
{
    x11_native_handle* native_handle = (x11_native_handle*)window->nativeHandle;

    //define events
    XEvent e;
    GRESSLIB_InputEvent ev;

    while (XPending(native_handle->display))
    {
        ev.eventType = EVENT_NONE;
        XNextEvent(native_handle->display, &e);
        switch (e.type)
        {
            default:
                break;
            case ClientMessage:
            {
             	//if the (x) button is clicked
             	if (e.xclient.data.l[0] == native_handle->delete_window)
		{
                    return GRESSLIB_PROCESSOSEVENTS_QuitEvent;
		}
		break;
            }
            case KeyPress:
	    case KeyRelease:
            {
                ev.eventType = e.type == KeyPress ? KEY_PRESS : KEY_RELEASE;
                ev.keycode = x_key_to_gresslib_key(XkbKeycodeToKeysym(native_handle->display, e.xkey.keycode, 0, 0));
                break;
            }
            case ButtonPress:
	    case ButtonRelease:
            {
                ev.eventType = e.type == ButtonPress ? MOUSEBUTTON_PRESS : MOUSEBUTTON_RELEASE;
                switch (e.xbutton.button)
                {
                    default:
                        ev.mouseButton = -1;
                        break;
                    case 1:
		    case 2:
		    case 3:
		    case 4:
		    case 5:
                        ev.mouseButton = e.xbutton.button;
                        break;
                }
                break;
            }
            case MotionNotify:
            {
                ev.eventType = MOUSE_MOVE;
                ev.mouseX = e.xmotion.x;
                ev.mouseY = e.xmotion.y;
                break;
            }
        }
        GRESSLIB_RunInputEventCallback(window, &ev);
    }
    return GRESSLIB_PROCESSOSEVENTS_NoQuitEvent;
}

void GRESSLIB_ShowCursor(GRESSLIB_Window * const window)
{
    x11_native_handle* native = (x11_native_handle*)window->nativeHandle;

    //define the default cursor as the current one
    XDefineCursor(native->display, native->window, native->normal);
    XFlush(native->display); 
}

void GRESSLIB_HideCursor(GRESSLIB_Window * const window)
{
    x11_native_handle* native = (x11_native_handle*)window->nativeHandle;

    //define the transparent cursor as the current one
    XDefineCursor(native->display, native->window, native->hidden);
    XFlush(native->display);
}

void GRESSLIB_WarpCursor(GRESSLIB_Window * const window, const int x, const int y)
{
    x11_native_handle* native = (x11_native_handle*)window->nativeHandle;
    XWarpPointer(native->display, None, native->window, 0, 0, 0, 0, x, y);
    XFlush(native->display);
}

enum GRESSLIB_KeyboardKeycode x_key_to_gresslib_key(KeySym keysym)
{
    //evil switch-case statement of death
    switch (keysym)
    {
    default:    return KEYCODE_UNDEFINED;
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
