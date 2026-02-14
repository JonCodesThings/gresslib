#ifndef GRESSLIB_INTERNAL_H
#define GRESSLIB_INTERNAL_H

#include <include/gresslib/gresslib.h>

// Useful internal helpers
// Lets us reuse code across platforms by not tying them to a specific backend implementation

GRESSLIB_Window* GRESSLIB_AllocateWindow(GRESSLIB_WindowDescriptor* const window_descriptor);

void GRESSLIB_SetInputEventCallback(GRESSLIB_Window* const window, const enum GRESSLIB_InputEventType event, GRESSLIB_InputEventCallback callback);

void GRESSLIB_RunInputEventCallback(GRESSLIB_Window* const window, GRESSLIB_InputEvent* const event);

// Logging helpers

#ifdef GRESSLIB_DEBUG
#include <assert.h>
#include <stdio.h>
#define gresslib_debug(str) do { printf("%s\n", str); } while(0);
#define gresslib_debug_only(...) __VA_ARGS__
#define gresslib_debugf(format, ...) do { printf(format, __VA_ARGS__); printf("\n"); } while(0);
#define gresslib_assert(cond, str) do { if (!(cond)) { gresslib_debug(str); } assert(cond); } while(0);
#define gresslib_assertf(cond, format, ...) do { if (!(cond)) { gresslib_debugf(format, __VA_ARGS__); } assert(cond); } while(0);
#else
#define gresslib_debug(str)
#define gresslib_debug_only(...)
#define gresslib_debugf(format, ...)
#define gresslib_assert(cond, format)
#define gresslib_assertf(cond, format, ...)
#endif

#endif
