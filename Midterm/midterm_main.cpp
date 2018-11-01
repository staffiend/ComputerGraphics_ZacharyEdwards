#include <iostream>
#include <assert.h>
#include <time.h>
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

int getTileID( int y, int x, int **table );

int main( int argc, char **argv )
{
	srand( time( 0 ) );

	//agent files
	const char *mmx_image_filename = "x1_sheet.png";
	const char *mmx_fx_filename = "mmx_fx_1.wav";

	const char *ken_sprites = "ken.png";

	const char *chun_sprites = "chun.png";

	const char *charlotte_sprites = "PoR_Charlotte.png";

	const char *dracula_sprites = "vlad_tepes.png";

	//tileset file
	const char *tileset_filename = "midterm_map.png";

	//music files
	const char *music_filenames[5] = { "targets.mp3", "yoshi_island.mp3", "earthbound.mp3", "dream_land.mp3", "bloody_tears.mp3" };


	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	Mix_OpenAudio( 22050, AUDIO_S16SYS, 2, 2048 );

	Mix_Music *music[5] = { Mix_LoadMUS( music_filenames[0] ), Mix_LoadMUS( music_filenames[1] ) , Mix_LoadMUS( music_filenames[2] ) , Mix_LoadMUS( music_filenames[3] ), Mix_LoadMUS( music_filenames[4] ) };

	Mix_Chunk *mmx_fx = Mix_LoadWAV(mmx_fx_filename);

	SDL_Window *window = SDL_CreateWindow( "Party!!!!!!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Game::screen_width, Game::screen_height, SDL_WINDOW_SHOWN );

	Game::renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	//load image into a surface
	SDL_Surface *tileset_surface = IMG_Load( tileset_filename );
	assert( tileset_surface );

	//create a texture from the surface
	SDL_Texture *tileset_texture = SDL_CreateTextureFromSurface( Game::renderer, tileset_surface );

	//free the surface as it is not needed anymore
	SDL_FreeSurface( tileset_surface );

	//states
	enum{idle, moving_left, moving_right, dancing, other};

	float state_table[5][5] =
	{
	{ 0.0, 0.2, 0.2, 0.5, 0.1 },
	{ 0.1, 0.1, 0.2, 0.5, 0.1 },
	{ 0.1, 0.2, 0.1, 0.5, 0.1 },
	{ 0.5, 0.1, 0.1, 0.2, 0.1 },
	{ 0.7, 0.1, 0.1, 0.1, 0.0 }
	};

	int my_state = 0;
	int time_since_state_change = 0;
	bool ending_state = false;

	//initialize agents, animations, and particle effects
	Vec2D f_min = { 0.0, -0.001 };
	Vec2D f_max = { 0.1, -0.005 };
	Particle_Emitter::Particle_Emitter fog_machine;
	Particle_Emitter::init( &fog_machine, 1000 );
	fog_machine.emitter_pos.x = 0;
	fog_machine.emitter_pos.y = 675;
	fog_machine.particle_color = { 200, 200, 200 };
	fog_machine.particle_size = 5;

	Vec2D s_min = { -0.5, -0.1 };
	Vec2D s_max = { 0.5, 0.1 };
	Particle_Emitter::Particle_Emitter sparks_one;
	Particle_Emitter::init( &sparks_one, 20 );
	sparks_one.emitter_pos.x = 120;
	sparks_one.emitter_pos.y = 100;
	sparks_one.particle_color = { 220, 220, 0 };
	sparks_one.particle_size = 2.5;

	Particle_Emitter::Particle_Emitter sparks_two;
	Particle_Emitter::init( &sparks_two, 20 );
	sparks_two.emitter_pos.x = 860;
	sparks_two.emitter_pos.y = 100;
	sparks_two.particle_color = { 240, 150, 0 };
	sparks_two.particle_size = 2.5;

	Particle_Emitter::Particle_Emitter sparks_three;
	Particle_Emitter::init( &sparks_three, 30 );
	sparks_three.emitter_pos.x = 510;
	sparks_three.emitter_pos.y = 100;
	sparks_three.particle_color = { 245, 245, 0 };
	sparks_three.particle_size = 2;

	Particle_Emitter::Particle_Emitter sparks_four;
	Particle_Emitter::init( &sparks_four, 30 );
	sparks_four.emitter_pos.x = 510;
	sparks_four.emitter_pos.y = 100;
	sparks_four.particle_color = { 255, 190, 0 };
	sparks_four.particle_size = 2;

	Vec2D b_min = { -0.001, 0.01 };
	Vec2D b_max = { 0.001, 0.1 };
	Particle_Emitter::Particle_Emitter blood_one;
	Particle_Emitter::init( &blood_one, 10 );
	blood_one.emitter_pos.x = 630;
	blood_one.emitter_pos.y = 169;
	blood_one.particle_color = { 200, 0, 0 };
	blood_one.particle_size = 3;

	Particle_Emitter::Particle_Emitter blood_two;
	Particle_Emitter::init( &blood_two, 10 );
	blood_two.emitter_pos.x = 665;
	blood_two.emitter_pos.y = 169;
	blood_two.particle_color = { 200, 0, 0 };
	blood_two.particle_size = 3;

	Vec2D w_min = { -0.01, -0.05 };
	Vec2D w_max = { 0.01, -0.08 };
	Particle_Emitter::Particle_Emitter fountain;
	Particle_Emitter::init( &fountain, 1000 );
	fountain.emitter_pos.x = 80;
	fountain.emitter_pos.y = 585;
	fountain.particle_color = { 0, 50, 220 };
	fountain.particle_size = 1.25;
	
	//mega man x agent and animations
	Agent::Agent mmx;
	Agent::init( &mmx, mmx_image_filename );
	mmx.pos = { 100, 150 };
	mmx.size.w = 48;
	mmx.size.h = 48;
	mmx.mass = 10;
	mmx.yOffset = -24;
	mmx.xOffset = 0;
	Animation::Animation mmx_idle;
	Animation::init( &mmx_idle, 0, 0, 3, 34, 44, 250 );
	Animation::Animation mmx_dance;
	Animation::init( &mmx_dance, 0, 132, 9, 34, 44, 100 );
	Animation::Animation mmx_move;
	Animation::init( &mmx_move, 0, 44, 10, 34, 44, 100 );
	Animation::Animation mmx_other;
	Animation::init( &mmx_other, 0, 88, 5, 34, 44, 100 );

	//ken agent and animations
	Agent::Agent ken;
	Agent::init( &ken, ken_sprites );
	ken.pos = { 150, 555 };
	ken.size.w = 100;
	ken.size.h = 120;
	ken.mass = 15;
	Animation::Animation ken_idle;
	Animation::init( &ken_idle, 0, 0, 10, 140, 120, 100 );
	Animation::Animation ken_move;
	Animation::init( &ken_move, 0, 120, 11, 140, 120, 100 );
	Animation::Animation ken_dance;
	Animation::init( &ken_dance, 0, 240, 8, 140, 120, 100 );
	Animation::Animation ken_other;
	Animation::init( &ken_other, 0, 360, 6, 140, 120, 100 );

	//chun agent and animations
	Agent::Agent chun;
	Agent::init( &chun, chun_sprites );
	chun.pos = { 300, 555 };
	chun.size.w = 80;
	chun.size.h = 120;
	chun.mass = 12;
	Animation::Animation chun_idle;
	Animation::init( &chun_idle, 0, 0, 10, 190, 120, 100 );
	Animation::Animation chun_move;
	Animation::init( &chun_move, 0, 120, 9, 190, 120, 100 );
	Animation::Animation chun_dance;
	Animation::init( &chun_dance, 0, 240, 10, 190, 120, 100 );
	Animation::Animation chun_other;
	Animation::init( &chun_other, 0, 360, 20, 190, 120, 100 );

	//charlotte agent and animations
	Agent::Agent charlotte;
	Agent::init( &charlotte, charlotte_sprites );
	charlotte.pos = { 700, 100 };
	charlotte.size.w = 48;
	charlotte.size.h = 32;
	charlotte.mass = 7;
	charlotte.yOffset = 8;
	Animation::Animation charlotte_idle;
	Animation::init( &charlotte_idle, 0, 0, 6, 36, 52, 100 );
	Animation::Animation charlotte_move;
	Animation::init( &charlotte_move, 0, 52, 10, 36, 52, 100 );
	Animation::Animation charlotte_dance;
	Animation::init( &charlotte_dance, 0, 104, 6, 36, 52, 100 );
	Animation::Animation charlotte_other;
	Animation::init( &charlotte_other, 0, 156, 4, 36, 52, 100 );

	Vec2D m_min = { -0.005, -0.007 };
	Vec2D m_max = { 0.005, -0.009 };
	Particle_Emitter::Particle_Emitter magic;
	Particle_Emitter::init( &magic, 150 );
	magic.emitter_pos.x = charlotte.pos.x + 30;
	magic.emitter_pos.y = charlotte.pos.y - 10;
	magic.particle_color = { 200, 0, 250 };
	magic.particle_size = 1;

	//dracula agent and animations
	Agent::Agent dracula;
	Agent::init( &dracula, dracula_sprites );
	dracula.pos = { 400, 350 };
	dracula.size.w = 50;
	dracula.size.h = 100;
	Animation::Animation vlad_idle;
	Animation::init( &vlad_idle, 0, 0, 7, 100, 100, 100 );

	int tile_size = 32;

	Vec2D gravity = { 0.0, 0.0005 };
	Vec2D move_force_right = { 0.0005, 0.0 };
	Vec2D move_force_left = { -0.0005, 0.0 };

	unsigned char prev_key_state[256];
	unsigned char *keys = (unsigned char*)SDL_GetKeyboardState( NULL );

	int current_time = 0;
	int time_since_song_change = 0;
	int playlist_index = 0;

	//initialize tilemap
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

	bool isGrounded = false;

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

		if ( ending_state == false )
		{
			if ( current_time - time_since_song_change > 10000 )
			{
				Mix_HaltMusic();
				playlist_index++;
				time_since_song_change = current_time;
			}
		}

		if ( playlist_index > 4 )
		{
			playlist_index = 0;
		}

		if ( Mix_PlayingMusic() == 0 )
		{
			Mix_PlayMusic( music[playlist_index], -1 );
		}

		if ( playlist_index == 4 )
		{
			ending_state = true;
		}

		/*
		GAME CODE
		*/

		//every frame
		//clear forces
		Particle_Emitter::clear_Forces_from_Particles( &fog_machine );
		Particle_Emitter::clear_Forces_from_Particles( &sparks_one );
		Particle_Emitter::clear_Forces_from_Particles( &sparks_two );
		Particle_Emitter::clear_Forces_from_Particles( &blood_one );
		Particle_Emitter::clear_Forces_from_Particles( &blood_two );
		Particle_Emitter::clear_Forces_from_Particles( &fountain );

		mmx.force = {};
		ken.force = {};
		chun.force = {};
		charlotte.force = {};

		//apply gravity
		Particle_Emitter::add_Force_to_Particles( &sparks_one, gravity );
		Particle_Emitter::add_Force_to_Particles( &sparks_two, gravity );
		Particle_Emitter::add_Force_to_Particles( &blood_one, gravity );
		Particle_Emitter::add_Force_to_Particles( &blood_two, gravity );
		Particle_Emitter::add_Force_to_Particles( &fountain, gravity );

		if ( mmx.isGrounded == false )
		{
			Agent::add_force( &mmx, gravity );
		}
		if ( ken.isGrounded == false )
		{
			Agent::add_force( &ken, gravity );
		}
		if ( chun.isGrounded == false )
		{
			Agent::add_force( &chun, gravity );
		}
		if ( charlotte.isGrounded == false )
		{
			Agent::add_force( &charlotte, gravity );
		}

		//apply moving forces
		if ( my_state == moving_left )
		{
			Agent::add_force( &mmx, move_force_left );
			Agent::add_force( &ken, move_force_left );
			Agent::add_force( &chun, move_force_left );
			Agent::add_force( &charlotte, move_force_left );
		}
		else if ( my_state == moving_right )
		{
			Agent::add_force( &mmx, move_force_right );
			Agent::add_force( &ken, move_force_right );
			Agent::add_force( &chun, move_force_right );
			Agent::add_force( &charlotte, move_force_right );
		}
		else
		{
			mmx.velocity.x = 0;
			ken.velocity.x = 0;
			chun.velocity.x = 0;
			charlotte.velocity.x = 0;
		}

		//spawn particles and physics/collision updates
		Particle_Emitter::spawn_Many( &fog_machine, f_min, f_max, 1, 500, 2000 );
		Particle_Emitter::update( &fog_machine, 1.0 );

		Particle_Emitter::spawn_Many( &sparks_one, s_min, s_max, 1, 20, 40 );
		Particle_Emitter::update( &sparks_one, 1.0 );

		Particle_Emitter::spawn_Many( &sparks_two, s_min, s_max, 1, 20, 40 );
		Particle_Emitter::update( &sparks_two, 1.0 );

		Particle_Emitter::spawn_Many( &sparks_three, s_min, s_max, 2, 5, 10 );
		Particle_Emitter::update( &sparks_three, 1.0 );

		Particle_Emitter::spawn_Many( &sparks_four, s_min, s_max, 2, 5, 10 );
		Particle_Emitter::update( &sparks_four, 1.0 );

		Particle_Emitter::spawn_Many( &blood_one, b_min, b_max, 1, 10, 20 );
		Particle_Emitter::update( &blood_one, 1.0 );

		Particle_Emitter::spawn_Many( &blood_two, b_min, b_max, 1, 10, 20 );
		Particle_Emitter::update( &blood_two, 1.0 );

		Particle_Emitter::spawn_Many( &fountain, w_min, w_max, 100, 200, 350 );
		Particle_Emitter::update( &fountain, 1.0 );

		Particle_Emitter::spawn_Many( &magic, m_min, m_max, 25, 25, 50 );
		Particle_Emitter::update( &magic, 1.0 );

		Agent::update( &mmx, 40.0 );
		Agent::collision_update( &mmx, 40.0, col_map );

		Agent::update( &ken, 40.0 );
		Agent::collision_update( &ken, 40.0, col_map );

		Agent::update( &chun, 40.0 );
		Agent::collision_update( &chun, 40.0, col_map );

		Agent::update( &charlotte, 40.0 );
		Agent::collision_update( &charlotte, 40.0, col_map );
		magic.emitter_pos.x = charlotte.pos.x + 45;
		magic.emitter_pos.y = charlotte.pos.y + 10;

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

		Particle_Emitter::draw( &blood_one, Game::renderer );
		Particle_Emitter::draw( &blood_two, Game::renderer );
		Particle_Emitter::draw( &fountain, Game::renderer );

		if ( ending_state == false )
		{
			if ( current_time - time_since_state_change > 2000 )
			{
				double r = ((double)rand() / (RAND_MAX));

				time_since_state_change = current_time;

				double sum = 0.0;
				for ( int j = 0; j < 5; j++ )
				{
					sum += state_table[my_state][j];
					if ( sum >= r )
					{
						my_state = j;
						break;
					}
				}
			}

			if ( my_state == idle )
			{
				Animation::PlayLoop( &mmx_idle, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_idle.width * 2, mmx_idle.height * 2, current_time );
				Animation::PlayLoop( &ken_idle, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_idle.width, ken_idle.height, current_time );
				Animation::PlayLoop( &chun_idle, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_idle.width, chun_idle.height, current_time );
				Animation::PlayLoop( &charlotte_idle, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_idle.width * 2, charlotte_idle.height * 2, current_time );
			}
			if ( my_state == dancing )
			{
				Animation::PlayLoop( &mmx_dance, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_dance.width * 2, mmx_dance.height * 2, current_time );
				Animation::PlayLoop( &ken_dance, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_dance.width, ken_dance.height, current_time );
				Animation::PlayLoop( &chun_dance, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_dance.width, chun_dance.height, current_time );
				Animation::PlayLoop( &charlotte_dance, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_dance.width * 2, charlotte_dance.height * 2, current_time );
			}
			if ( my_state == moving_left )
			{
				Animation::PlayLoopFlipped( &mmx_move, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_move.width * 2, mmx_move.height * 2, current_time );
				Animation::PlayLoop( &ken_move, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_move.width, ken_move.height, current_time );
				Animation::PlayLoop( &chun_move, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_move.width, chun_move.height, current_time );
				Animation::PlayLoop( &charlotte_move, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_move.width * 2, charlotte_move.height * 2, current_time );
			}
			if ( my_state == moving_right )
			{
				Animation::PlayLoop( &mmx_move, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_move.width * 2, mmx_move.height * 2, current_time );
				Animation::PlayLoopFlipped( &ken_move, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_move.width, ken_move.height, current_time );
				Animation::PlayLoopFlipped( &chun_move, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_move.width, chun_move.height, current_time );
				Animation::PlayLoopFlipped( &charlotte_move, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_move.width * 2, charlotte_move.height * 2, current_time );
			}
			if ( my_state == other )
			{
				Animation::PlayLoop( &mmx_other, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_other.width * 2, mmx_other.height * 2, current_time );
				Animation::PlayLoop( &ken_other, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_other.width, ken_other.height, current_time );
				Animation::PlayLoop( &chun_other, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_other.width, chun_other.height, current_time );
				Animation::PlayLoop( &charlotte_other, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_other.width * 2, charlotte_other.height * 2, current_time );
				Particle_Emitter::draw( &magic, Game::renderer );
				Mix_PlayChannel( -1, mmx_fx, 0 );
			}
		}

		if ( ending_state == true )
		{
			Animation::PlayLoop( &mmx_other, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_other.width * 2, mmx_other.height * 2, current_time );
			Animation::PlayLoop( &ken_other, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_other.width, ken_other.height, current_time );
			Animation::PlayLoop( &chun_other, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_other.width, chun_other.height, current_time );
			Animation::PlayLoop( &charlotte_other, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_other.width * 2, charlotte_other.height * 2, current_time );
			Animation::PlayLoop( &vlad_idle, dracula.sprite_texture, dracula.pos.x, dracula.pos.y, vlad_idle.width * 3, vlad_idle.height * 3, current_time );
			Particle_Emitter::draw( &magic, Game::renderer );
		}
		
		/*if ( my_state == idle )
		{
			Animation::PlayLoop( &mmx_idle, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_idle.width * 2, mmx_idle.height * 2, current_time );
			Animation::PlayLoop( &ken_idle, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_idle.width, ken_idle.height, current_time );
			Animation::PlayLoop( &chun_idle, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_idle.width, chun_idle.height, current_time );
			Animation::PlayLoop( &charlotte_idle, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_idle.width * 2, charlotte_idle.height * 2, current_time );
		}
		if ( my_state == dancing )
		{
			Animation::PlayLoop( &mmx_dance, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_dance.width * 2, mmx_dance.height * 2, current_time );
			Animation::PlayLoop( &ken_dance, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_dance.width, ken_dance.height, current_time );
			Animation::PlayLoop( &chun_dance, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_dance.width, chun_dance.height, current_time );
			Animation::PlayLoop( &charlotte_dance, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_dance.width * 2, charlotte_dance.height * 2, current_time );
		}
		if ( my_state == moving_left )
		{
			Animation::PlayLoopFlipped( &mmx_move, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_move.width * 2, mmx_move.height * 2, current_time );
			Animation::PlayLoop( &ken_move, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_move.width, ken_move.height, current_time );
			Animation::PlayLoop( &chun_move, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_move.width, chun_move.height, current_time );
			Animation::PlayLoop( &charlotte_move, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_move.width * 2, charlotte_move.height * 2, current_time );
		}
		if ( my_state == moving_right )
		{
			Animation::PlayLoop( &mmx_move, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_move.width * 2, mmx_move.height * 2, current_time );
			Animation::PlayLoopFlipped( &ken_move, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_move.width, ken_move.height, current_time );
			Animation::PlayLoopFlipped( &chun_move, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_move.width, chun_move.height, current_time );
			Animation::PlayLoopFlipped( &charlotte_move, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_move.width * 2, charlotte_move.height * 2, current_time );
		}
		if ( my_state == other )
		{
			Animation::PlayLoop( &mmx_other, mmx.sprite_texture, mmx.pos.x, mmx.pos.y, mmx_other.width * 2, mmx_other.height * 2, current_time );
			Animation::PlayLoop( &ken_other, ken.sprite_texture, ken.pos.x, ken.pos.y, ken_other.width, ken_other.height, current_time );
			Animation::PlayLoop( &chun_other, chun.sprite_texture, chun.pos.x, chun.pos.y, chun_other.width, chun_other.height, current_time );
			Animation::PlayLoop( &charlotte_other, charlotte.sprite_texture, charlotte.pos.x, charlotte.pos.y, charlotte_other.width * 2, charlotte_other.height * 2, current_time );
			Particle_Emitter::draw( &magic, Game::renderer );
			Mix_PlayChannel( -1, mmx_fx, 0 );
		}*/

		Particle_Emitter::draw( &fog_machine, Game::renderer );
		Particle_Emitter::draw( &sparks_one, Game::renderer );
		Particle_Emitter::draw( &sparks_two, Game::renderer );
		Particle_Emitter::draw( &sparks_three, Game::renderer );
		Particle_Emitter::draw( &sparks_four, Game::renderer );

		//flip buffers
		SDL_RenderPresent( Game::renderer );
	}

	return 0;
}

