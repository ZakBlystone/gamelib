#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lib.h"
#include "draw.h"

static gamelib_t g_game_lib = {0};
static libgfx_t g_gfx_lib = {0};
static libutil_t g_util_lib = {0};

static callback_start g_cb_start = NULL;
static callback_loop g_cb_loop = NULL;
static callback_resize g_cb_resize = NULL;
static callback_stop g_cb_stop = NULL;
static callback_mousebutton g_cb_mousebutton = NULL;
static callback_mousemove g_cb_mousemove = NULL;
static callback_mouseenter g_cb_mouseenter = NULL;
static callback_keyboard g_cb_keyboard = NULL;

static GLFWwindow* window = NULL;
static float g_time = 0.f;
static float g_deltatime = 0.f;

static void gl_reshape(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	glLoadIdentity();
	glOrtho(0,width,height,0,-1,1);

	if ( g_cb_resize != NULL )
	{
		g_cb_resize( width, height );
	}
}

static void _mouse_button(GLFWwindow* window, int button, int action, int mods)
{
	if ( g_cb_mousebutton ) g_cb_mousebutton(button, action != 0, mods);
}

static void _mouse_move(GLFWwindow* window, double x, double y)
{
	if ( g_cb_mousemove ) g_cb_mousemove((float) x, (float) y);
}

static void _mouse_enter(GLFWwindow* window, int enter)
{
	if ( g_cb_mouseenter ) g_cb_mouseenter(enter == GLFW_TRUE);
}

static void _keyboard(GLFWwindow* window, int key, int scan, int action, int mods)
{
	if ( g_cb_keyboard ) g_cb_keyboard(key, scan, action != 0, mods);
}

static bool _init(const initparams_t* params)
{
	if ( !glfwInit() ) return false;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);

	g_cb_start = params->cb_start;
	g_cb_loop = params->cb_loop;
	g_cb_resize = params->cb_resize;
	g_cb_stop = params->cb_stop;

	g_cb_mousebutton = params->cb_mousebutton;
	g_cb_mousemove = params->cb_mousemove;
	g_cb_mouseenter = params->cb_mouseenter;
	g_cb_keyboard = params->cb_keyboard;

	window = glfwCreateWindow(
		params->width, 
		params->height, 
		params->title, NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, gl_reshape);
	glfwSetMouseButtonCallback(window, _mouse_button);
	glfwSetCursorPosCallback(window, _mouse_move);
	glfwSetCursorEnterCallback(window, _mouse_enter);
	glfwSetKeyCallback(window, _keyboard);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	if ( !window )
	{
		glfwTerminate();
		return 1;
	}

	gl_reshape(window, params->width, params->height);

	init_gfx_lib(&g_gfx_lib);
	g_game_lib.gfx = &g_gfx_lib;
	g_game_lib.util = &g_util_lib;

	g_time = (float)glfwGetTime();
	g_deltatime = 0.f;

	if ( g_cb_start && !g_cb_start() ) return false;

	return true;
}

static bool _update(void)
{
	float last_time = g_time;
	g_time = (float)glfwGetTime();
	g_deltatime = g_time - last_time;

	if ( !glfwWindowShouldClose(window) )
	{
		glClearColor(.1f, .15f, .3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if ( g_cb_loop && !g_cb_loop() ) return false;

		glfwSwapBuffers(window);
		glfwPollEvents();
		return true;
	}

	return false;
}

static void _shutdown(void)
{
	if ( g_cb_stop ) g_cb_stop();

	g_game_lib.gfx = NULL;
	g_game_lib.util = NULL;
	shutdown_gfx_lib();

	glfwTerminate();
}

static float _get_time(void)
{
	return g_time;
}

static float _get_deltatime(void)
{
	return g_deltatime;
}

__declspec(dllexport) gamelib_t* get_game_lib(void)
{
	g_game_lib.init = _init;
	g_game_lib.update = _update;
	g_game_lib.shutdown = _shutdown;

	g_util_lib.get_time = _get_time;
	g_util_lib.get_deltatime = _get_deltatime;

	return &g_game_lib;
}