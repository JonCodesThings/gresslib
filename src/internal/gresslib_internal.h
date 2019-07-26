#ifndef GRESSLIB_INTERNAL_H
#define GRESSLIB_INTERNAL_H

#include <include/gresslib/gresslib.h>

//common useful internal functions

/*!
Function that allocates a window struct.
author: Jonathan Duncanson
*/
window* allocate_window(window_descriptor* const window_descriptor);

/*!
Internal function to set input even callbacks.
author: Jonathan Duncanson
*/
void set_input_event_callback(window* const window, const enum input_events event, input_callback callback);

/*!
Internal function to set run a specific input event callback.
author: Jonathan Duncanson
*/
void run_input_event_callback(window* const window, input_event* const event);

#endif
