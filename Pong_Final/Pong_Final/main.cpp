/*Zachary Edwards*/

#include <iostream>
#include <assert.h>
#include <ctime>
using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")

#pragma comment(linker,"/subsystem:console")


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

	Ball ball = { 250, 250, 25, 25, speed_coefficient * (1.0 - 2.0*rand() / RAND_MAX), speed_coefficient * (1.0 - 2.0*rand() / RAND_MAX) };
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
		ball.xPos += ball.speedX;
		ball.yPos += ball.speedY;

		//have paddles follow ball
		if ( ball.xPos < screen_width * 0.75 && ball.xPos > leftPaddle.xPos + leftPaddle.width )
		{
			if ( ball.yPos < leftPaddle.yPos + (leftPaddle.height / 2) )
			{
				leftPaddle.yPos -= leftPaddle.speedY;
			}
			else if ( ball.yPos > leftPaddle.yPos + (leftPaddle.height / 2) )
			{
				leftPaddle.yPos += leftPaddle.speedY;
			}
		}

		if ( ball.xPos > screen_width * 0.25 && ball.xPos < rightPaddle.xPos )
		{
			if ( ball.yPos < rightPaddle.yPos + (rightPaddle.height / 2) )
			{
				rightPaddle.yPos -= rightPaddle.speedY;
			}
			else if ( ball.yPos > rightPaddle.yPos + (rightPaddle.height / 2) )
			{
				rightPaddle.yPos += rightPaddle.speedY;
			}
		}

		//keep ball on screen and check for collision with paddles
		if ( (ball.xPos < 0) || (ball.xPos + ball.width > screen_width) )
		{
			ball.speedX *= -1;
		}
		if ( (ball.yPos < 0) || (ball.yPos + ball.height > screen_height) )
		{
			ball.speedY *= -1;
		}
		if ( check_collision( ball, leftPaddle ) || (check_collision( ball, rightPaddle )) )
		{
			ball.speedX *= -1;
			ball.speedY *= -1;
			ball.speedX += (ball.speedX * 0.01);
			ball.speedY += (ball.speedY * 0.01);
		}

		//set color to white
		SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

		//clear screen with white
		SDL_RenderClear( renderer );

		//set color to green
		SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );

		SDL_Rect ballRect;
		ballRect.x = ball.xPos;
		ballRect.y = ball.yPos;
		ballRect.w = ball.width;
		ballRect.h = ball.height;

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
		SDL_RenderFillRect( renderer, &ballRect );
		SDL_RenderFillRect( renderer, &leftPaddleRect );
		SDL_RenderFillRect( renderer, &rightPaddleRect );

		//flip buffers
		SDL_RenderPresent( renderer );
	}

	return 0;
}