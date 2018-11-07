#pragma once
#include "basic_data.h"
#include "game.h"
#include "animation.h"
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"

int getTileID( int y, int x, int **table )
{
	if ( x >= 0 && x < 30 && y >= 0 && y < 24 )
	{
		return table[y][x];
	}
	else
	{
		return 1000;
	}
}

namespace Agent
{
	struct Agent
	{
		Vec2D pos;
		Vec2D nextPos;
		Size size;
		Vec2D force;
		Vec2D velocity;
		float mass;
		int xOffset;
		int yOffset;
		int state;
		int time_since_state_change;
		bool isGrounded;
		bool has_sound_played;
		const char *sprite_filename;
		SDL_Surface *sprite_surface;
		SDL_Texture *sprite_texture;
	};

	void init( Agent *a, const char *filename )
	{
		a->pos = {};
		a->nextPos = {};
		a->size = {};
		a->force = {};
		a->velocity = {};
		a->mass = 1.0;
		a->xOffset = 0;
		a->yOffset = 0;
		a->state = 0;
		a->time_since_state_change = 0;
		a->isGrounded = false;
		a->has_sound_played = false;
		a->sprite_surface = IMG_Load( filename );
		a->sprite_texture = SDL_CreateTextureFromSurface( Game::renderer, a->sprite_surface );
		SDL_FreeSurface( a->sprite_surface );
	}

	void update( Agent *a, float time )
	{
		Vec2D acceleration;
		acceleration.x = a->force.x / a->mass;
		acceleration.y = a->force.y / a->mass;
		a->velocity.x += acceleration.x * time;
		a->velocity.y += acceleration.y * time;
		a->pos.x += a->velocity.x * time;
		a->pos.y += a->velocity.y * time;
	}

	void add_force( Agent *a, Vec2D force )
	{
		a->force.x += force.x / a->mass;
		a->force.y += force.y / a->mass;
	}

	void collision_update( Agent *a, float time, int **table )
	{
		a->nextPos.x = a->pos.x + a->velocity.x * time;
		a->nextPos.y = a->pos.y + a->velocity.y * time;

		int xCell = (int)(a->pos.x + a->size.w + a->xOffset) / 30;
		int yCell = (int)(a->pos.y + a->size.h + a->yOffset) / 24;

		if ( a->pos.x <= 0 || a->pos.x + a->size.w > Game::screen_width )
		{
			a->velocity.x *= -1;
		}

		//this always causes a collision when moving sideways, probably because it registers a collision with the bottom tiles constantly, don't know how to fix yet
		if ( a->velocity.x <= 0 )
		{
			if ( getTileID( yCell + 0.0, xCell + 0.0, table ) != -1 || getTileID( yCell + 0.0, xCell + 0.9, table ) != -1 )
			{
				//a->velocity.x = 0.0;
			}
		}
		else
		{
			if ( getTileID( yCell + 1.0, xCell, table ) != -1 || getTileID( yCell + 1.0, xCell + 0.9, table ) != -1 )
			{
				//a->velocity.x = 0.0;
			}
		}

		if ( a->velocity.y <= 0 )
		{
			if ( getTileID( yCell, xCell, table ) != -1 || getTileID( yCell, xCell, table ) != -1 )
			{
			}
		}
		else
		{
			if ( getTileID( yCell, xCell, table ) != -1 || getTileID( yCell, xCell, table ) != -1 )
			{
				a->pos.y = a->pos.y;
				a->velocity.y = 0.0;
				a->isGrounded = true;
			}
		}
	}
	
}
