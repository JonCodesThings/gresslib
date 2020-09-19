#include <include/gresslib/gresslib.h>

#include <src/win32/win32_internal.h>

#include <windows.h>

bool GRESSLIB_BootstrapGL(GRESSLIB_Window* const window, GRESSLIB_GLContextDescriptor* const context_desc)
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
	pfd.cColorBits = context_desc->redSize + context_desc->greenSize + context_desc->blueSize + context_desc->alphaSize;

	//R, G, B and A bit counts
	pfd.cRedBits = context_desc->redSize;
	pfd.cGreenBits = context_desc->greenSize;
	pfd.cBlueBits = context_desc->blueSize;
	pfd.cAlphaBits = context_desc->alphaSize;

	//depth and stencil bit sizes
	pfd.cDepthBits = context_desc->depthSize;
	pfd.cStencilBits = context_desc->stencilSize;

	win32_native_handle *native_handle = (win32_native_handle*)window->nativeHandle;

	native_handle->hdc = GetDC(native_handle->wnd);

	int index = ChoosePixelFormat(native_handle->hdc, &pfd);
	SetPixelFormat(native_handle->hdc, index, &pfd);

	native_handle->gl_context = wglCreateContext(native_handle->hdc);
	wglMakeCurrent(native_handle->hdc, native_handle->gl_context);

	return true;
}

void GRESSLIB_SwapGLBuffers(GRESSLIB_Window* const window)
{
	win32_native_handle* native = (win32_native_handle*)window->nativeHandle;

	wglSwapLayerBuffers(native->hdc, WGL_SWAP_MAIN_PLANE);
}

void GRESSLIB_ShutdownGL(GRESSLIB_Window* window)
{
	win32_native_handle *native = (win32_native_handle*)window->nativeHandle;

	wglDeleteContext(native->gl_context);
}