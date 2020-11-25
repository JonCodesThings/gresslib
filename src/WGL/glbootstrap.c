#include <include/gresslib/gresslib.h>

#include <src/win32/win32_internal.h>

#include <windows.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB   0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

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

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0, 0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	if (wglCreateContextAttribsARB != NULL)
	{
		HGLRC modernContext = wglCreateContextAttribsARB(native_handle->hdc, NULL, attribs);
		if (modernContext != NULL)
		{
			wglMakeCurrent(native_handle->hdc, modernContext);
			wglDeleteContext(native_handle->gl_context);
			native_handle->gl_context = modernContext;
		}
	}
	else
		return false;

	return true;
}

void GRESSLIB_SwapGLBuffers(GRESSLIB_Window* const window)
{
	win32_native_handle* native = (win32_native_handle*)window->nativeHandle;

	wglSwapLayerBuffers(native->hdc, WGL_SWAP_MAIN_PLANE);
	//SwapBuffers(native->hdc);
}

void GRESSLIB_ShutdownGL(GRESSLIB_Window* window)
{
	win32_native_handle *native = (win32_native_handle*)window->nativeHandle;

	wglDeleteContext(native->gl_context);
}