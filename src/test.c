//create bootstrap code for loading DLL (init_game_lib, free_game_lib)
#define GAMELIB_WITH_BOOTSTRAP

#include <stdio.h>
#include <math.h>
#include "lib.h"

//library struct, all library functions are in here
static gamelib_t* lib;

//player state
enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };
typedef struct
{
	float x, y, vx, vy;
	bool move[4];
} player_t;

static texture_t my_texture;
static font_t my_font;
static player_t player = {0};

static float mx = 0;
static float my = 0;
static float fieldwidth = 0;
static float fieldheight = 0;

static void resize(int width, int height) { fieldwidth = width; fieldheight = height; }

//called on startup right after the library initializes, create things here
static bool start(void)
{
	my_texture = lib->gfx->load_texture("Gear.png");
	my_font = lib->gfx->load_font("NotoMono-Regular.ttf");

	player.x = fieldwidth/2.f;
	player.y = fieldheight/2.f;

	//return false to stop game from starting (error handling)
	return true;
}

//called on game shutdown, use this to free resources
static void stop(void)
{
	lib->gfx->free_texture( my_texture );
	lib->gfx->free_font( my_font );
}

//called every frame
static bool loop(void)
{
	static const float speed = 1200.f;
	static const float friction = 3.f;

	float t = lib->util->get_time();
	float dt = lib->util->get_deltatime();

	//accelerate player
	if ( player.move[MOVE_UP] ) player.vy -= speed * dt;
	if ( player.move[MOVE_DOWN] ) player.vy += speed * dt;
	if ( player.move[MOVE_LEFT] ) player.vx -= speed * dt;
	if ( player.move[MOVE_RIGHT] ) player.vx += speed * dt;

	player.x += player.vx * dt;
	player.y += player.vy * dt;

	//clamp player to play area
	if ( player.x > fieldwidth ) { player.x = fieldwidth; player.vx *= -1.f; }
	if ( player.x < 0.f ) { player.x = 0.f; player.vx *= -1.f; }
	if ( player.y > fieldheight ) { player.y = fieldheight; player.vy *= -1.f; }
	if ( player.y < 0.f ) { player.y = 0.f; player.vy *= -1.f; }

	//apply friction
	player.vx *= expf(dt * -friction);
	player.vy *= expf(dt * -friction);

	//draw everything
	lib->gfx->set_color(COLOR3F(1,.5f + sinf(t) * .5f,1));
	lib->gfx->set_texture(my_texture);
	lib->gfx->draw_sprite(player.x, player.y, 32,32,0);
	lib->gfx->set_color(COLOR3F(1,1,1));
	lib->gfx->draw_text(my_font, mx, my+24, "Testing graphics");

	//returning false ends the game loop
	return true;
}

//keep track of where the mouse is
static void mouse_moved(float x, float y)
{
	mx = x;
	my = y;
}

//called when a key is pressed or released
static void keyboard(int key, int scancode, bool pressed, int modifiers)
{
	if ( key == KEY_W ) player.move[MOVE_UP] = pressed;
	if ( key == KEY_S ) player.move[MOVE_DOWN] = pressed;
	if ( key == KEY_A ) player.move[MOVE_LEFT] = pressed;
	if ( key == KEY_D ) player.move[MOVE_RIGHT] = pressed;
}

int main(int argc, const char**argv)
{
	initparams_t params = {0};

	//load the game library dll
	if ( !init_game_lib() ) return 1;

	//get handle to library struct
	lib = get_game_lib();

	//intialization parameters, define all callbacks here
	params.title = "Library Test";
	params.width = 800;
	params.height = 600;
	params.cb_resize = resize;
	params.cb_start = start;
	params.cb_loop = loop;
	params.cb_stop = stop;
	params.cb_mousemove = mouse_moved;
	params.cb_keyboard = keyboard;
	if ( !lib->init( &params ) )
	{
		printf("Error initializing game library\n");
		return 1;
	}

	//game loop
	while ( lib->update() );

	//shutdown to call all finalizers
	lib->shutdown();

	//free game library
	free_game_lib();
	return 0;
}