#include <stdlib.h>

#include <include/gresslib/gresslib.h>

struct window* allocate_window(struct window_descriptor* const window_descriptor)
{
    void* window_alloc = malloc(sizeof(struct window));

	if (window_alloc == NULL)
		return NULL;

    struct window* wnd = (struct window*)window_alloc;
    wnd->descriptor = *window_descriptor;

    return wnd;
}