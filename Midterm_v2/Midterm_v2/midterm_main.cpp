#include <iostream>
#include <assert.h>
#include <time.h>
//#include <SDL.h>
//#include <SDL_image.h>
//#include <SDL_mixer.h>
#include <fstream>
#include "basic_data.h"
#include "game.h"
#include "agent.h"
#include "particles.h"
#include "animation.h"
#include "tileset.h"
#include "Table_File_core.h"
using namespace std;


#include "SDL2-2.0.8\include\SDL_mixer.h"
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_mixer.lib")

#pragma comment(linker,"/subsystem:console")


int main( int argc, char **argv )
{
	srand( time( 0 ) );

	const char *mmx_image_filename = "x1_sheet.png";
	const char *tileset_filename = "midterm_map.png";
	const char *music_filenames[4] = { "targets.mp3", "yoshi_island.mp3", "earthbound.mp3", "dream_land.mp3" };

	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	Mix_OpenAudio( 22050, AUDIO_S16SYS, 2, 2048 );

	Mix_Music *music[4] = { Mix_LoadMUS( music_filenames[0] ), Mix_LoadMUS( music_filenames[1] ) , Mix_LoadMUS( music_filenames[2] ) , Mix_LoadMUS( music_filenames[3] ) };

	SDL_Window *window = SDL_CreateWindow( "Party!!!!!!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Game::screen_width, Game::screen_height, SDL_WINDOW_SHOWN );

	Game::renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	//load image into a surface
	SDL_Surface *tileset_surface = IMG_Load( tileset_filename );
	assert( tileset_surface );

	//create a texture from the surface
	SDL_Texture *tileset_texture = SDL_CreateTextureFromSurface( Game::renderer, tileset_surface );

	//free the surface as it is not needed anymore
	SDL_FreeSurface( tileset_surface );

	enum{idle, moving, dancing};

	Vec2D f_min = { 0.0, -0.001 };
	Vec2D f_max = { 0.1, -0.005 };
	Particle_Emitter::Particle_Emitter fog_machine;
	Particle_Emitter::init( &fog_machine, 1000 );
	fog_machine.emitter_pos.x = 0;
	fog_machine.emitter_pos.y = 500;
	fog_machine.particle_color = { 200, 200, 200 };
	fog_machine.particle_size = 5;

	Vec2D s_min = { -0.05, 0.001 };
	Vec2D s_max = { 0.05, 0.01 };
	Particle_Emitter::Particle_Emitter sparks_one;
	Particle_Emitter::init( &sparks_one, 50 );
	sparks_one.emitter_pos.x = 320;
	sparks_one.emitter_pos.y = 0;
	sparks_one.particle_color = { 220, 220, 0 };
	sparks_one.particle_size = 5;

	Particle_Emitter::Particle_Emitter sparks_two;
	Particle_Emitter::init( &sparks_two, 50 );
	sparks_two.emitter_pos.x = 640;
	sparks_two.emitter_pos.y = 0;
	sparks_two.particle_color = { 240, 150, 0 };
	sparks_two.particle_size = 5;

	Animation::Animation mmx_idle;
	Animation::init( &mmx_idle, 0, 0, 3, 34, 44, 250 );

	Animation::Animation x1_anim;
	Animation::init( &x1_anim, 0, 132, 9, 34, 44, 100 );

	Agent::Agent mmx;
	Agent::init( &mmx, mmx_image_filename );
	mmx.size.w = 32;
	mmx.size.h = 48;
	mmx.mass = 10;
	mmx.state = idle;

	int tile_size = 32;

	Vec2D gravity = { 0.0, 0.0005 };

	unsigned char prev_key_state[256];
	unsigned char *keys = (unsigned char*)SDL_GetKeyboardState( NULL );

	int current_time = 0;
	int playlist_index = 0;

	Tileset::Tileset tileset;
	tileset.tile_width = 32;
	tileset.tile_height = 32;
	tileset.n_cols = 30;

	Table_File::Table_File map_background;
	Table_File::read( "midterm_map_v3_background.csv", &map_background );

	int **bg_map = new int *[map_background.nrows];
	for ( int i = 0; i < map_background.nrows; i++ )
	{
		bg_map[i] = new int[map_background.ncols[i]];

		for ( int j = 0; j < map_background.ncols[i]; j++ )
		{
			bg_map[i][j] = atoi( map_background.table[i][j] );
		}
	}

	int bg_map_n_cols = map_background.ncols[0];
	int bg_map_n_rows = map_background.nrows;

	Table_File::Table_File map_collision;
	Table_File::read( "midterm_map_v3_collision.csv", &map_collision );

	int **col_map = new int *[map_collision.nrows];
	for ( int i = 0; i < map_collision.nrows; i++ )
	{
		col_map[i] = new int[map_collision.ncols[i]];

		for ( int j = 0; j < map_collision.ncols[i]; j++ )
		{
			col_map[i][j] = atoi( map_collision.table[i][j] );
		}
	}

	int col_map_n_cols = map_collision.ncols[0];
	int col_map_n_rows = map_collision.nrows;

	bool done = false;
	while ( !done )
	{
		Uint32 current_time = SDL_GetTicks();
		memcpy( prev_key_state, keys, 256 );

		//consume all window events first
		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
			{
				done = true;
			}
			else if ( event.type == SDL_KEYDOWN )
			{
				if ( event.key.keysym.sym == SDLK_n )
				{
					Mix_HaltMusic();
					playlist_index++;
					mmx.state = dancing;
				}
				if ( event.key.keysym.sym == SDLK_m )
				{
					mmx.state = idle;
				}
			}
		}

		int cmd_up = 0;
		int cmd_down = 0;
		int cmd_right = 0;
		int cmd_left = 0;

		if ( keys[SDL_SCANCODE_W] ) cmd_up = 1;
		if ( keys[SDL_SCANCODE_S] ) cmd_down = 1;
		if ( keys[SDL_SCANCODE_A] ) cmd_left = 1;
		if ( keys[SDL_SCANCODE_D] ) cmd_right = 1;

		if ( playlist_index > 3 )
		{
			playlist_index = 0;
		}

		if ( Mix_PlayingMusic() == 0 )
		{
			Mix_PlayMusic( music[playlist_index], -1 );
		}

		/*
		GAME CODE
		*/

		//every frame
		//clear forces
		Particle_Emitter::clear_Forces_from_Particles( &fog_machine );
		Particle_Emitter::clear_Forces_from_Particles( &sparks_one );
		Particle_Emitter::clear_Forces_from_Particles( &sparks_two );
		mmx.force = {};

		//apply gravity
		Particle_Emitter::add_Force_to_Particles( &sparks_one, gravity );
		Particle_Emitter::add_Force_to_Particles( &sparks_two, gravity );
		//Agent::add_force( &mmx, gravity );

		Particle_Emitter::spawn_Many( &fog_machine, f_min, f_max, 1, 500, 2000 );
		Particle_Emitter::update( &fog_machine, 1.0 );

		Particle_Emitter::spawn_Many( &sparks_one, s_min, s_max, 1, 100, 200 );
		Particle_Emitter::update( &sparks_one, 1.0 );

		Particle_Emitter::spawn_Many( &sparks_two, s_min, s_max, 1, 100, 200 );
		Particle_Emitter::update( &sparks_two, 1.0 );

		Agent::physics_update( &mmx, 40.0 );

		//RENDER

		//set color to white
		SDL_SetRenderDrawColor( Game::renderer, 200, 200, 200, 200 );
		//clear screen with white
		SDL_RenderClear( Game::renderer );

		for ( int i = 0; i < bg_map_n_rows; i++ )
		{
			for ( int j = 0; j < bg_map_n_cols; j++ )
			{
				int tile_id = bg_map[i][j];

				SDL_Rect src;
				src.x = Tileset::get_Col( tile_id, &tileset );
				src.y = Tileset::get_Row( tile_id, &tileset );
				src.w = tile_size;
				src.h = tile_size;

				SDL_Rect dest;
				dest.x = j * tileset.tile_width;
				dest.y = i * tileset.tile_height;
				dest.w = tile_size;
				dest.h = tile_size;

				SDL_RenderCopyEx( Game::renderer, tileset_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE );
			}
		}

		for ( int i = 0; i < col_map_n_rows; i++ )
		{
			for ( int j = 0; j < col_map_n_cols; j++ )
			{
				int tile_id = col_map[i][j];

				SDL_Rect src;
				src.x = Tileset::get_Col( tile_id, &tileset );
				src.y = Tileset::get_Row( tile_id, &tileset );
				src.w = tile_size;
				src.h = tile_size;

				SDL_Rect dest;
				dest.x = j * tileset.tile_width;
				dest.y = i * tileset.tile_height;
				dest.w = tile_size;
				dest.h = tile_size;

				SDL_RenderCopyEx( Game::renderer, tileset_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE );
			}
		}

		if ( mmx.state == idle )
		{
			Animation::PlayLoop( &mmx_idle, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_idle.width * 2, mmx_idle.height * 2, current_time );
		}
		if ( mmx.state == dancing )
		{
			Animation::PlayLoop( &x1_anim, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, x1_anim.width * 2, x1_anim.height * 2, current_time );
		}

		Particle_Emitter::draw( &fog_machine, Game::renderer );
		Particle_Emitter::draw( &sparks_one, Game::renderer );
		Particle_Emitter::draw( &sparks_two, Game::renderer );

		//flip buffers
		SDL_RenderPresent( Game::renderer );

	}

	return 0;
}

