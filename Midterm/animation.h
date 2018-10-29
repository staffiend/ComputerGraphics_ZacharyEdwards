#pragma once
#include "basic_data.h"
#include "game.h"
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"


namespace Animation
{
	struct Animation
	{
		Vec2D starting_pos;
		int frames;
		float width;
		float height;
		int frame_duration;
		int current_frame;
		int last_frame_change_time;
	};

	void init( Animation* anim, float starting_xPos, float starting_yPos, int n_frames, int width, int height, int duration )
	{
		anim->starting_pos = { starting_xPos, starting_yPos };
		anim->frames = n_frames;
		anim->width = width;
		anim->height = height;
		anim->frame_duration = duration;
		anim->current_frame = 0;
		anim->last_frame_change_time = 0;
	}

	void PlayLoop( Animation *anim, SDL_Texture *texture, float dest_x, float dest_y, int dest_width, int dest_height, int current_time )
	{
		if ( current_time - anim->last_frame_change_time > anim->frame_duration )
		{
			anim->current_frame = (anim->current_frame + 1) % anim->frames;
			anim->last_frame_change_time = current_time;
		}

		SDL_Rect src = { anim->starting_pos.x + anim->current_frame * anim->width, anim->starting_pos.y, anim->width, anim->height };
		SDL_Rect dest = { dest_x, dest_y, dest_width, dest_height };


		SDL_RenderCopyEx( Game::renderer, texture, &src, &dest, 0, NULL, SDL_FLIP_NONE );
	}

	void PlayOnce( Animation *anim, SDL_Texture *texture, float dest_x, float dest_y, int dest_width, int dest_height, int current_time )
	{
		if ( current_time - anim->last_frame_change_time > anim->frame_duration && anim->current_frame < anim->frames )
		{
			anim->current_frame = (anim->current_frame + 1);
			anim->last_frame_change_time = current_time;
		}

		SDL_Rect src = { anim->starting_pos.x + anim->current_frame * anim->width, anim->starting_pos.y, anim->width, anim->height };
		SDL_Rect dest = { dest_x, dest_y, dest_width, dest_height };


		SDL_RenderCopyEx( Game::renderer, texture, &src, &dest, 0, NULL, SDL_FLIP_NONE );
	}

}
