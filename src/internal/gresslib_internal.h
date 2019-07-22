#ifndef GRESSLIB_INTERNAL_H
#define GRESSLIB_INTERNAL_H

#include <include/gresslib/gresslib.h>

struct window* allocate_window(struct window_descriptor* const window_descriptor);

void set_input_event_callback(struct window* const window, const enum input_events event, input_callback callback);

void run_input_event_callback(struct window* const window, struct input_event* const event);

#endif