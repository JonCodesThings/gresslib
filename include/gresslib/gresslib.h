#ifndef GRESSLIB_H
#define GRESSLIB_H

//Function pointer typedefs for memory functions
typedef void* (*GRESSLIB_ALLOC)(size_t);
typedef void (*GRESSLIB_DEALLOC)(void*);

extern GRESSLIB_ALLOC GRESSLIB_Allocate;
extern GRESSLIB_DEALLOC GRESSLIB_Deallocate;

void GRESSLIB_SetAllocator(GRESSLIB_ALLOC const alloc, GRESSLIB_DEALLOC const dealloc);

// Enum to conveniently define window styles
enum GRESSLIB_WindowStyle
{
	WINDOW_BORDERED = 0,
	WINDOW_BORDERLESS = 1 << 0,
	WINDOW_FULLSCREEN = 1 << 1,
	WINDOW_RESIZEABLE = 1 << 2,
	WINDOW_CLOSEABLE = 1 << 3
};

// Enum to define input event types
enum GRESSLIB_InputEventType
{
	EVENT_NONE = 0,
	KEY_PRESS = 1 << 0,
	KEY_RELEASE = 1 << 1,
	MOUSE_MOVE = 1 << 2,
	MOUSEBUTTON_PRESS = 1 << 3,
	MOUSEBUTTON_RELEASE = 1 << 4,
	MOUSEWHEEL_MOVE = 1 << 5
};

// Enum to define keyboard keycodes
enum GRESSLIB_KeyboardKeycode
{
	KEYCODE_UNDEFINED = -1,
	NUM_ZERO = 0,
	NUM_ONE = 1,
	NUM_TWO = 2,
	NUM_THREE = 3,
	NUM_FOUR = 4,
	NUM_FIVE = 5,
	NUM_SIX = 6,
	NUM_SEVEN = 7,
	NUM_EIGHT = 8,
	NUM_NINE = 9,
	Q = 10,
	W = 11,
	E = 12,
	R = 13,
	T = 14,
	Y = 15,
	U = 16,
	I = 17,
	O = 18,
	P = 19,
	A = 20,
	S = 21,
	D = 22,
	F = 23,
	G = 24,
	H = 25,
	J = 26,
	K = 27,
	L = 28,
	Z = 29,
	X = 30,
	C = 31,
	V = 32,
	B = 33,
	N = 34,
	M = 35,
	GRAVE = 36,
	HYPHEN = 37,
	EQUAL = 38,
	BACKSPACE = 39,
	TAB = 40,
	LEFT_SQUARE_BRACKET = 41,
	RIGHT_SQUARE_BRACKET = 42,
	ENTER = 43,
	CAPS_LOCK = 44,
	SEMICOLON = 45,
	SINGLE_QUOTE = 46,
	HASH = 47,
	LEFT_SHIFT = 48,
	COMMA = 49,
	FULLSTOP = 50,
	FORWARD_SLASH = 51,
	RIGHT_SHIFT = 52,
	LEFT_CONTROL = 53,
	LEFT_ALT = 54,
	SPACEBAR = 55,
	RIGHT_ALT = 56,
	RIGHT_CONTROL = 57,
	ESCAPE = 58,
	F1 = 59,
	F2 = 60,
	F3 = 61,
	F4 = 62,
	F5 = 63,
	F6 = 64,
	F7 = 65,
	F8 = 66,
	F9 = 67,
	F10 = 68,
	F11 = 69,
	F12 = 70
};

// Struct to store input event data
typedef struct
{
	enum GRESSLIB_InputEventType eventType;
	union
	{
		enum GRESSLIB_KeyboardKeycode keycode;
		unsigned int mouseButton;
		int mouseWheelDelta;
		struct
		{
			int mouseX;
			int mouseY;
		};
	};
} GRESSLIB_InputEvent;

//typedef for input callbacks
typedef void (*GRESSLIB_InputEventCallback)(GRESSLIB_InputEvent*);

// Struct to define window properties
typedef struct
{
	const char* title;
	unsigned int width;
	unsigned int height;
	unsigned int style;
} GRESSLIB_WindowDescriptor;

// Enum to define an actual window
typedef struct
{
	GRESSLIB_WindowDescriptor descriptor;
	void* nativeHandle;
	GRESSLIB_InputEventCallback onKeyPress;
	GRESSLIB_InputEventCallback onKeyRelease;
	GRESSLIB_InputEventCallback onMouseMove;
	GRESSLIB_InputEventCallback onMouseButtonPress;
	GRESSLIB_InputEventCallback onMouseButtonRelease;
	GRESSLIB_InputEventCallback onMouseWheelMove;
} GRESSLIB_Window;

// Struct to define OpenGL context properties
typedef struct
{
	unsigned int redSize;
	unsigned int blueSize;
	unsigned int greenSize;
	unsigned int alphaSize;
	unsigned int depthSize;
	unsigned int stencilSize;
} GRESSLIB_GLContextDescriptor;

// Allocates and creates a window
// Returns a valid pointer on success, NULL on failure
GRESSLIB_Window *GRESSLIB_CreateWindow(GRESSLIB_WindowDescriptor * const windowDesc);

enum GRESSLIB_DestroyWindowResult
{
	GRESSLIB_DESTROYWINDOW_Failed,
	GRESSLIB_DESTROYWINDOW_Success
};

// Destroys a given window
// This deallocates the passed in GRESSLIB_Window
enum GRESSLIB_DestroyWindowResult GRESSLIB_DestroyWindow(GRESSLIB_Window * const window);

enum GRESSLIB_ProcessOSEventsResult
{
	GRESSLIB_PROCESSOSEVENTS_NoQuitEvent,
	GRESSLIB_PROCESSOSEVENTS_QuitEvent
};

// Processes OS-level window events
enum GRESSLIB_ProcessOSEventsResult GRESSLIB_ProcessOSEvents(GRESSLIB_Window * const window);

enum GRESSLIB_BootstrapGLResult
{
	GRESSLIB_BOOTSTRAPGL_FAILED,
	GRESSLIB_BOOTSTRAPGL_SUCCESS
};

// Creates an OpenGL context based on the passed in context descriptor
enum GRESSLIB_BootstrapGLResult GRESSLIB_BootstrapGL(GRESSLIB_Window * const window, GRESSLIB_GLContextDescriptor* const contextDesc);

// Function that swaps the backbuffer of the current active OpenGL context.
void GRESSLIB_SwapGLBuffers(GRESSLIB_Window * const window);

// Function that sets the callback for the relevant input event type.
void GRESSLIB_SetInputEventCallback(GRESSLIB_Window * const window, const enum GRESSLIB_InputEventType event, GRESSLIB_InputEventCallback callback);

void GRESSLIB_ShowCursor(GRESSLIB_Window * const window);

void GRESSLIB_HideCursor(GRESSLIB_Window * const window);

void GRESSLIB_WarpCursor(GRESSLIB_Window * const window, const int x, const int y);

#endif
