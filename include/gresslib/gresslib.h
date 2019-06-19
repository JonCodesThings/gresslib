#include <stdbool.h>

enum window_style
{
	WINDOW_BORDERED = 0,
	WINDOW_BORDERLESS = 1 << 0,
	WINDOW_FULLSCREEN = 1 << 1,
	WINDOW_RESIZEABLE = 1 << 2,
	WINDOW_CLOSEABLE = 1 << 3
};

enum input_events
{
	KEY_PRESS = 1,
	KEY_RELEASE = 2,
	MOUSE_MOVE = 3,
	MOUSEBUTTON_PRESS = 4,
	MOUSEBUTTON_RELEASE = 5,
	MOUSEWHEEL_MOVE = 6
};

enum keyboard_keycodes
{
	UNDEFINED = -1,
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

struct input_event
{
	enum input_events event_type;
	union
	{
		enum keyboard_keycodes keycode;
		unsigned int mouse_button;
		int mouse_wheel_delta;
		struct
		{
			int mouse_x;
			int mouse_y;
		};
	};
};

typedef void (*input_callback)(struct input_event*);

struct window_descriptor
{
	const char* title;
	unsigned int width;
	unsigned int height;
	unsigned int style;
};

struct window
{
	struct window_descriptor descriptor;
	void* native_handle;
	input_callback on_key_press;
	input_callback on_key_release;
	input_callback on_mouse_move;
	input_callback on_mouse_button_press;
	input_callback on_mouse_button_release;
	input_callback on_mouse_wheel_move;
};

struct window* create_window(struct window_descriptor* const window_desc);

bool destroy_window(struct window* window);

bool process_os_events(struct window* const window);

void set_input_event_callback(struct window* const window, const enum input_events event, input_callback callback);