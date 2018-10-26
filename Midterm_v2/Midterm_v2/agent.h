#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "basic_data.h"
#include "game.h"

namespace Agent
{
	struct Agent
	{
		Vec2D pos;
		Size size;
		Vec2D force;
		Vec2D velocity;
		float mass;
		int state;
		const char *sprite_filename;
		SDL_Surface *sprite_surface;
		SDL_Texture *sprite_texture;
	};

	void init( Agent *a, const char *filename )
	{
		a->pos = { 50, 50 };
		a->size = {};
		a->force = {};
		a->velocity = {};
		a->mass = 1.0;
		a->state = 0;
		a->sprite_surface = IMG_Load( filename );
		a->sprite_texture = SDL_CreateTextureFromSurface( Game::renderer, a->sprite_surface );
		SDL_FreeSurface( a->sprite_surface );
	}

	void physics_update( Agent *a, float time )
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
		a->force.x += force.x;
		a->force.y += force.y;
	}
}
