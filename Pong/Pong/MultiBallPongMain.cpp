/*Zachary Edwards*/

#include <iostream>
#include <assert.h>
#include <SDL.h>
#include <ctime>
using namespace std;

/*
I included this just in case.

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")

#pragma comment(linker,"/subsystem:console")
*/

struct Ball
{
	float xPos, yPos;
	float width, height;
	float speedX, speedY;
};

struct Paddle
{
	float xPos, yPos;
	float width, height;
	float speedY;
};

bool check_collision( Ball A, Paddle B )
{
	//if any of the sides from A are outside of B
	//bottom of A, top of B
	if ( A.yPos + A.height < B.yPos )
	{
		return false;
	}

	//top of A, bottom of B
	if ( A.yPos > B.yPos + B.height )
	{
		return false;
	}

	//right side of A, left side of B
	if ( A.xPos + A.width < B.xPos )
	{
		return false;
	}

	//left side of A, right side of B
	if ( A.xPos > B.xPos + B.width )
	{
		return false;
	}

	//if none of the sides from A are outside B
	return true;
}


int main( int argc, char **argv )
{
	srand( time( NULL ) );

	SDL_Init( SDL_INIT_VIDEO );

	int screen_width = 800;
	int screen_height = 600;

	SDL_Window *window = SDL_CreateWindow( "Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN );

	SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

	float speed_coefficient = 0.1;

	int n_balls = 200;
	Ball *balls = new Ball[n_balls];

	for ( int i = 0; i < n_balls; i++ )
	{
		balls[i].xPos = screen_width / 2;
		balls[i].yPos = screen_height / 2;

		balls[i].width = 25;
		balls[i].height = 25;

		balls[i].speedX = speed_coefficient * (1.0 - 2.0*rand() / RAND_MAX);
		balls[i].speedY = speed_coefficient * (1.0 - 2.0*rand() / RAND_MAX);
	}

	Paddle leftPaddle = { 70, 50, 20, 100, 0.05 };
	Paddle rightPaddle = { 710, 50, 20, 100, 0.05 };

	bool done = false;
	while ( !done )
	{
		//consume all window events first
		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
			{
				done = true;
			}
		}

		/*GAME CODE*/

		//UPDATE
		for ( int i = 0; i < n_balls; i++ )
		{
			balls[i].xPos += balls[i].speedX;
			balls[i].yPos += balls[i].speedY;

			//have paddles follow ball
			if ( balls[i].xPos < screen_width * 0.75 && balls[i].xPos > leftPaddle.xPos + leftPaddle.width )
			{
				if ( balls[i].yPos < leftPaddle.yPos + ( leftPaddle.height / 2 ) )
				{
					leftPaddle.yPos -= leftPaddle.speedY;
				}
				else if ( balls[i].yPos > leftPaddle.yPos + ( leftPaddle.height / 2 ) )
				{
					leftPaddle.yPos += leftPaddle.speedY;
				}
			}

			if ( balls[i].xPos > screen_width * 0.25 && balls[i].xPos < rightPaddle.xPos )
			{
				if ( balls[i].yPos < rightPaddle.yPos + ( rightPaddle.height / 2 ) )
				{
					rightPaddle.yPos -= rightPaddle.speedY;
				}
				else if ( balls[i].yPos > rightPaddle.yPos + ( rightPaddle.height / 2) )
				{
					rightPaddle.yPos += rightPaddle.speedY;
				}
			}
		}
		
		//keep ball on screen and check for collision with paddles
		for ( int i = 0; i < n_balls; i++ )
		{
			if ( (balls[i].xPos < 0) || (balls[i].xPos + balls[i].width > screen_width) )
			{
				balls[i].speedX *= -1;
			}
			if ( (balls[i].yPos < 0) || (balls[i].yPos + balls[i].height > screen_height) )
			{
				balls[i].speedY *= -1;
			}
			if ( check_collision( balls[i], leftPaddle ) || (check_collision( balls[i], rightPaddle ) ) )
			{
				balls[i].speedX *= -1;
				balls[i].speedY *= -1;
				balls[i].speedX += (balls[i].speedX * 0.01);
				balls[i].speedY += (balls[i].speedY * 0.01);
			}
		}
		
		//set color to white
		SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

		//clear screen with white
		SDL_RenderClear( renderer );

		//set color to green
		SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );

		for ( int i = 0; i < n_balls; i++ )
		{
			SDL_Rect ballRect;
			ballRect.x = balls[i].xPos;
			ballRect.y = balls[i].yPos;
			ballRect.w = balls[i].width;
			ballRect.h = balls[i].height;

			SDL_RenderFillRect( renderer, &ballRect );
		}

		SDL_Rect leftPaddleRect;
		leftPaddleRect.x = leftPaddle.xPos;
		leftPaddleRect.y = leftPaddle.yPos;
		leftPaddleRect.w = leftPaddle.width;
		leftPaddleRect.h = leftPaddle.height;

		SDL_Rect rightPaddleRect;
		rightPaddleRect.x = rightPaddle.xPos;
		rightPaddleRect.y = rightPaddle.yPos;
		rightPaddleRect.w = rightPaddle.width;
		rightPaddleRect.h = rightPaddle.height;

		//draw filled rectangles in the backbuffer
		SDL_RenderFillRect( renderer, &leftPaddleRect );
		SDL_RenderFillRect( renderer, &rightPaddleRect );

		//flip buffers
		SDL_RenderPresent( renderer );
	}

	return 0;
}