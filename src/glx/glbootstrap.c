#include <include/gresslib/gresslib.h>

#include <src/x11/x11_internal.h>

#include <GL/glx.h>

//TODO add proper context handling using structs(?) - Jon

bool bootstrap_gl(window* window, glcontext_descriptor* const context_desc)
{
    //context attribute array
    //TODO support more context details
    int context_attribs[] =
    {
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_redSize, context_desc->redSize,
        GLX_greenSize, context_desc->greenSize,
        GLX_blueSize, context_desc->blueSize,
        GLX_alphaSize, context_desc->alphaSize,
        GLX_STENCIL_SIZE, context_desc->stencil_size,
        GLX_DOUBLEBUFFER, True,
        None
    };

    //get the native handle
    x11_native_handle* native = (x11_native_handle*)window->native_handle;

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

    native->gl_context = context;

    return true;
}

void swap_gl_buffers(window* const window)
{
    x11_native_handle* native = (x11_native_handle*)window->native_handle;
    
    glXSwapBuffers(native->display, native->window);
}

void shutdown_gl(window* window)
{
    x11_native_handle* native = (x11_native_handle*)window->native_handle;

    glXDestroyContext(native->display, native->gl_context);
}