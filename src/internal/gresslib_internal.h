#ifndef GRESSLIB_INTERNAL_H
#define GRESSLIB_INTERNAL_H

#include <include/gresslib/gresslib.h>

// Useful internal helpers
// Let's us reuse code across platforms by not tying them to a specific backend implementation

GRESSLIB_Window* GRESSLIB_AllocateWindow(GRESSLIB_WindowDescriptor* const window_descriptor);

void GRESSLIB_SetInputEventCallback(GRESSLIB_Window* const window, const enum GRESSLIB_InputEventType event, GRESSLIB_InputEventCallback callback);

void GRESSLIB_RunInputEventCallback(GRESSLIB_Window* const window, GRESSLIB_InputEvent* const event);

#endif
