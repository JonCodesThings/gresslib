#include <include/gresslib/gresslib.h>

#include <src/win32/win32_internal.h>

#include <windows.h>

bool bootstrap_gl(window* const window, glcontext_descriptor* const context_desc)
{
	//set up the pixel format descriptor
	//TODO support more context details
	PIXELFORMATDESCRIPTOR pfd;

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);

	//set to RGBA
	pfd.iPixelType = PFD_TYPE_RGBA;

	//generic PFD flags required for the GL stuff
	//TODO: figure out what we actually need here
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	//framebuffer bits
	pfd.cColorBits = context_desc->red_size + context_desc->green_size + context_desc->blue_size + context_desc->alpha_size;

	//R, G, B and A bit counts
	pfd.cRedBits = context_desc->red_size;
	pfd.cGreenBits = context_desc->green_size;
	pfd.cBlueBits = context_desc->blue_size;
	pfd.cAlphaBits = context_desc->alpha_size;

	//depth and stencil bit sizes
	pfd.cDepthBits = context_desc->depth_size;
	pfd.cStencilBits = context_desc->stencil_size;

	win32_native_handle *native_handle = (win32_native_handle*)window->native_handle;

	native_handle->hdc = GetDC(native_handle->wnd);

	int index = ChoosePixelFormat(native_handle->hdc, &pfd);
	SetPixelFormat(native_handle->hdc, index, &pfd);

	native_handle->gl_context = wglCreateContext(native_handle->hdc);
	wglMakeCurrent(native_handle->hdc, native_handle->gl_context);

	return true;
}

void swap_gl_buffers(window* const window)
{
	win32_native_handle* native = (win32_native_handle*)window->native_handle;

	wglSwapLayerBuffers(native->hdc, WGL_SWAP_MAIN_PLANE);
}

void shutdown_gl(window* window)
{
	win32_native_handle *native = (win32_native_handle*)window->native_handle;

	wglDeleteContext(native->gl_context);
}