#ifndef GRESSLIB_X11_INTERNAL_H
#define GRESSLIB_X11_INTERNAL_H

//lovingly ripped from  
// https://www.cl.cam.ac.uk/~mgk25/ucs/keysym2ucs.c
//author: Markus G. Kuhn <http://www.cl.cam.ac.uk/~mgk25/> University of Cambridge, April 2001

#include <X11/XKBlib.h>

struct x11_native_handle
{
    Display* display;
    Window window;
    Atom delete_window;
    Atom window_hints;
};

long keysym2ucs(KeySym keysym);

#endif