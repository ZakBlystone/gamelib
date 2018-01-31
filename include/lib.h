#ifndef GAMELIB_H
#define GAMELIB_H

#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif

#ifndef NULL
#define NULL 0
#endif

typedef enum
{
	BLEND_ALPHA,
	BLEND_ADD,
} blend_t;

typedef void* handle_t;
typedef handle_t font_t;
typedef unsigned int texture_t;
typedef unsigned int color_t;

typedef bool (*callback_start)(void);
typedef bool (*callback_loop)(void);
typedef void (*callback_stop)(void);
typedef void (*callback_resize)(int width, int height);
typedef void (*callback_mousebutton)(int button, bool pressed, int modifiers);
typedef void (*callback_mousemove)(float x, float y);
typedef void (*callback_mouseenter)(bool entered);
typedef void (*callback_keyboard)(int key, int scancode, bool pressed, int modifiers);

typedef struct
{
	float x,y;
	float u,v;
} vertex_t;

typedef struct
{
	texture_t (*load_texture)(const char* filename);
	font_t (*load_font)(const char* filename);
	void (*free_texture)(texture_t texture);
	void (*free_font)(font_t font);

	void (*set_blend)(blend_t blendmode);
	void (*set_texture)(texture_t texture);
	void (*set_color)(color_t color);

	void (*draw_rect)(float x, float y, float width, float height);
	void (*draw_sprite)(float x, float y, float width, float height, float rotation);
	void (*draw_quad)(vertex_t vertices[4]);
	void (*draw_polygon)(vertex_t* vertices, int num_vertices);
	void (*draw_text)(font_t font, float x, float y, const char* text);
} libgfx_t;

typedef struct
{
	float (*get_time)(void);
	float (*get_deltatime)(void);
} libutil_t;

typedef struct
{
	const char* title;
	int width;
	int height;
	callback_start cb_start;
	callback_loop cb_loop;
	callback_resize cb_resize;
	callback_stop cb_stop;
	callback_mousebutton cb_mousebutton;
	callback_mousemove cb_mousemove;
	callback_mouseenter cb_mouseenter;
	callback_keyboard cb_keyboard;
} initparams_t;

typedef struct
{
	bool (*init)(const initparams_t* params);
	bool (*update)(void);
	void (*shutdown)(void);
	libgfx_t* gfx;
	libutil_t* util;
} gamelib_t;

typedef gamelib_t* (*pfn_get_game_lib)(void);

#ifdef GAMELIB_WITH_BOOTSTRAP
#ifdef _WIN32

#include <stdio.h>
#include <windows.h>

static pfn_get_game_lib get_game_lib;
static HINSTANCE _game_lib_module = NULL;

static bool print_windows_error(void)
{
	char buffer[8192];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 8192, NULL );
	printf("Error loading library %s\n", buffer);
	return false;
}

static bool init_game_lib(void)
{
#ifdef _WIN64
	_game_lib_module = LoadLibrary("gamelib64.dll");
#else
	_game_lib_module = LoadLibrary("gamelib.dll");
#endif
	if ( _game_lib_module == NULL ) return print_windows_error();

	get_game_lib = (pfn_get_game_lib) GetProcAddress( _game_lib_module, "get_game_lib" );

	return get_game_lib != NULL;
}

static void free_game_lib(void)
{
	get_game_lib = NULL;

	FreeLibrary( _game_lib_module );
}

#endif //_WIN32
#endif //GAMELIB_WITH_BOOTSTRAP

//KEYS
#define KEY_UNKNOWN            -1
#define KEY_SPACE              32
#define KEY_APOSTROPHE         39  /* ' */
#define KEY_COMMA              44  /* , */
#define KEY_MINUS              45  /* - */
#define KEY_PERIOD             46  /* . */
#define KEY_SLASH              47  /* / */
#define KEY_0                  48
#define KEY_1                  49
#define KEY_2                  50
#define KEY_3                  51
#define KEY_4                  52
#define KEY_5                  53
#define KEY_6                  54
#define KEY_7                  55
#define KEY_8                  56
#define KEY_9                  57
#define KEY_SEMICOLON          59  /* ; */
#define KEY_EQUAL              61  /* = */
#define KEY_A                  65
#define KEY_B                  66
#define KEY_C                  67
#define KEY_D                  68
#define KEY_E                  69
#define KEY_F                  70
#define KEY_G                  71
#define KEY_H                  72
#define KEY_I                  73
#define KEY_J                  74
#define KEY_K                  75
#define KEY_L                  76
#define KEY_M                  77
#define KEY_N                  78
#define KEY_O                  79
#define KEY_P                  80
#define KEY_Q                  81
#define KEY_R                  82
#define KEY_S                  83
#define KEY_T                  84
#define KEY_U                  85
#define KEY_V                  86
#define KEY_W                  87
#define KEY_X                  88
#define KEY_Y                  89
#define KEY_Z                  90
#define KEY_LEFT_BRACKET       91  /* [ */
#define KEY_BACKSLASH          92  /* \ */
#define KEY_RIGHT_BRACKET      93  /* ] */
#define KEY_GRAVE_ACCENT       96  /* ` */
#define KEY_WORLD_1            161 /* non-US #1 */
#define KEY_WORLD_2            162 /* non-US #2 */
#define KEY_ESCAPE             256
#define KEY_ENTER              257
#define KEY_TAB                258
#define KEY_BACKSPACE          259
#define KEY_INSERT             260
#define KEY_DELETE             261
#define KEY_RIGHT              262
#define KEY_LEFT               263
#define KEY_DOWN               264
#define KEY_UP                 265
#define KEY_PAGE_UP            266
#define KEY_PAGE_DOWN          267
#define KEY_HOME               268
#define KEY_END                269
#define KEY_CAPS_LOCK          280
#define KEY_SCROLL_LOCK        281
#define KEY_NUM_LOCK           282
#define KEY_PRINT_SCREEN       283
#define KEY_PAUSE              284
#define KEY_F1                 290
#define KEY_F2                 291
#define KEY_F3                 292
#define KEY_F4                 293
#define KEY_F5                 294
#define KEY_F6                 295
#define KEY_F7                 296
#define KEY_F8                 297
#define KEY_F9                 298
#define KEY_F10                299
#define KEY_F11                300
#define KEY_F12                301
#define KEY_F13                302
#define KEY_F14                303
#define KEY_F15                304
#define KEY_F16                305
#define KEY_F17                306
#define KEY_F18                307
#define KEY_F19                308
#define KEY_F20                309
#define KEY_F21                310
#define KEY_F22                311
#define KEY_F23                312
#define KEY_F24                313
#define KEY_F25                314
#define KEY_KP_0               320
#define KEY_KP_1               321
#define KEY_KP_2               322
#define KEY_KP_3               323
#define KEY_KP_4               324
#define KEY_KP_5               325
#define KEY_KP_6               326
#define KEY_KP_7               327
#define KEY_KP_8               328
#define KEY_KP_9               329
#define KEY_KP_DECIMAL         330
#define KEY_KP_DIVIDE          331
#define KEY_KP_MULTIPLY        332
#define KEY_KP_SUBTRACT        333
#define KEY_KP_ADD             334
#define KEY_KP_ENTER           335
#define KEY_KP_EQUAL           336
#define KEY_LEFT_SHIFT         340
#define KEY_LEFT_CONTROL       341
#define KEY_LEFT_ALT           342
#define KEY_LEFT_SUPER         343
#define KEY_RIGHT_SHIFT        344
#define KEY_RIGHT_CONTROL      345
#define KEY_RIGHT_ALT          346
#define KEY_RIGHT_SUPER        347
#define KEY_MENU               348
#define KEY_LAST               KEY_MENU

#define KEYMOD_SHIFT           0x0001
#define KEYMOD_CONTROL         0x0002
#define KEYMOD_ALT             0x0004
#define KEYMOD_SUPER           0x0008
#define KEYMOD_CAPS_LOCK       0x0010
#define KEYMOD_NUM_LOCK        0x0020

#define MOUSE_BUTTON_1         0
#define MOUSE_BUTTON_2         1
#define MOUSE_BUTTON_3         2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3

inline color_t COLOR3(int r, int g, int b)
{
	return 0xFF000000 | ((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF);
}

inline color_t COLOR4(int r, int g, int b, int a)
{
	return ((a & 0xFF) << 24) | ((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF);
}

inline color_t COLOR3F(float r, float g, float b)
{
	return COLOR3( (int)(r * 255.f), (int)(g * 255.f), (int)(b * 255.f) );
}

inline color_t COLOR4F(float r, float g, float b, float a)
{
	return COLOR4( (int)(r * 255.f), (int)(g * 255.f), (int)(b * 255.f), (int)(a * 255.f) );
}

inline float DEGREES(float radians) { return radians * 57.3f; }
inline float RADIANS(float degrees) { return degrees / 57.3f; }

#endif //GAMELIB_H