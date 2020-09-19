#ifndef GRESSLIB_INTERNAL_H
#define GRESSLIB_INTERNAL_H

#include <include/gresslib/gresslib.h>

//common useful internal functions

/*!
Function that allocates a window struct.
author: Jonathan Duncanson
*/
GRESSLIB_Window* GRESSLIB_AllocateWindow(GRESSLIB_WindowDescriptor* const window_descriptor);

/*!
Internal function to set input even callbacks.
author: Jonathan Duncanson
*/
void GRESSLIB_SetInputEventCallback(GRESSLIB_Window* const window, const enum GRESSLIB_InputEventType event, GRESSLIB_InputEventCallback callback);

/*!
Internal function to set run a specific input event callback.
author: Jonathan Duncanson
*/
void GRESSLIB_RunInputEventCallback(GRESSLIB_Window* const window, GRESSLIB_InputEvent* const event);

#endif
