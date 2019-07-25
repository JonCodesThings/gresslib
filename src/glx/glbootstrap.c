#include <include/gresslib/gresslib.h>

#include <src/x11/x11_internal.h>

#include <GL/glx.h>

//TODO add proper context handling using structs(?) - Jon

bool bootstrap_gl(struct window* window, struct glcontext_descriptor* const context_desc)
{
    //context attribute array
    //TODO support more context details
    int context_attribs[] =
    {
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, context_desc->red_size,
        GLX_GREEN_SIZE, context_desc->green_size,
        GLX_BLUE_SIZE, context_desc->blue_size,
        GLX_ALPHA_SIZE, context_desc->alpha_size,
        GLX_STENCIL_SIZE, context_desc->stencil_size,
        GLX_DOUBLEBUFFER, True,
        None
    };

    //get the native handle
    struct x11_native_handle* native = (struct x11_native_handle*)window->native_handle;

    //choose visual info based on the context attribute array
    XVisualInfo* vis = glXChooseVisual(native->display, DefaultScreen(native->display), context_attribs);

    if (!vis)
        return false;

    //create a colormap for the window
    Colormap col = XCreateColormap(native->display, native->window, (Visual*)vis, AllocNone);

    //set the colormap for the window
    XSetWindowColormap(native->display, native->window, col);
    
    //create the OpenGL context
    GLXContext context = glXCreateContext(native->display, vis, NULL, GL_TRUE);

    if (!context)
        return false;

    //make the context current
    glXMakeCurrent(native->display, native->window, context);

    return true;
}

void swap_gl_buffers(struct window* window)
{
    struct x11_native_handle* native = (struct x11_native_handle*)window->native_handle;
    
    glXSwapBuffers(native->display, native->window);
}