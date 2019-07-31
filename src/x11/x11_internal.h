#ifndef GRESSLIB_X11_INTERNAL_H
#define GRESSLIB_X11_INTERNAL_H

#include <X11/XKBlib.h>

#include <GL/glx.h>

/*!
Struct for X11 native handles.
author: Jonathan Duncanson
*/
typedef struct
{
    Display* display;
    Window window;
    Cursor normal;
    Cursor hidden;
    Atom delete_window;
    Atom window_hints;
    GLXContext gl_context;
} x11_native_handle;

//lovingly ripped from  
// https://www.cl.cam.ac.uk/~mgk25/ucs/keysym2ucs.c
//author: Markus G. Kuhn <http://www.cl.cam.ac.uk/~mgk25/> University of Cambridge, April 2001
long keysym2ucs(KeySym keysym);

#endif
