#include <include/gresslib/gresslib.h>

#include <src/internal/gresslib_internal.h>
#include <src/x11/x11_internal.h>

#include <GL/glx.h>


enum GRESSLIB_BootstrapGLResult GRESSLIB_BootstrapGL(GRESSLIB_Window* window, GRESSLIB_GLContextDescriptor* const context_desc)
{
    //context attribute array
    //TODO support more context details
    int context_attribs[] =
    {
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, context_desc->redSize,
        GLX_GREEN_SIZE, context_desc->greenSize,
        GLX_BLUE_SIZE, context_desc->blueSize,
        GLX_ALPHA_SIZE, context_desc->alphaSize,
        GLX_STENCIL_SIZE, context_desc->stencilSize,
        GLX_DOUBLEBUFFER, True,
        None
    };

    //get the native handle
    x11_native_handle* native = (x11_native_handle*)window->nativeHandle;

    //choose visual info based on the context attribute array
    XVisualInfo* vis = glXChooseVisual(native->display, DefaultScreen(native->display), context_attribs);

    if (!vis)
    {
        gresslib_debug("BootstrapGL failed! glxChooseVisual returned NULL!");
        return GRESSLIB_BOOTSTRAPGL_FAILED;
    }

    //create a colormap for the window
    Colormap col = XCreateColormap(native->display, native->window, (Visual*)vis, AllocNone);

    //set the colormap for the window
    XSetWindowColormap(native->display, native->window, col);
    
    //create the OpenGL context
    GLXContext context = glXCreateContext(native->display, vis, NULL, GL_TRUE);

    if (!context)
    {
        gresslib_debug("BootstrapGL failed! glxCreateContext returned invalid GL context!");
        return GRESSLIB_BOOTSTRAPGL_FAILED;
    }

    //make the context current
    glXMakeCurrent(native->display, native->window, context);

    native->gl_context = context;

    XFree(vis);

    gresslib_debug("BootstrapGL success! GL context successfully created!");
    return GRESSLIB_BOOTSTRAPGL_SUCCESS;
}

void GRESSLIB_SwapGLBuffers(GRESSLIB_Window* const window)
{
    x11_native_handle* native = (x11_native_handle*)window->nativeHandle;
    
    glXSwapBuffers(native->display, native->window);
}

void GRESSLIB_ShutdownGL(GRESSLIB_Window* window)
{
    x11_native_handle* native = (x11_native_handle*)window->nativeHandle;

    glXDestroyContext(native->display, native->gl_context);
    gresslib_debug("ShutdownGL success! GL context successfully destroyed!");
}
