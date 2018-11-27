#pragma warning(disable:4996)
#pragma comment(linker, "/subsystem:console")

//load libraries
#pragma comment(lib, "SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib, "SDL2-2.0.8\\lib\\x86\\SDL2main.lib")

#pragma comment(lib, "glew-2.1.0\\lib\\Release\\Win32\\glew32.lib")
#pragma comment(lib, "opengl32.lib")

//headers
#include "glew-2.1.0\include\GL\\glew.h"
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_opengl.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

unsigned int vertex_buffer_object = 0;
unsigned int index_buffer_object = 0;
unsigned int vertex_array_object = 0;

const unsigned int screen_width = 800;
const unsigned int screen_height = 600;
SDL_Window *window = NULL;

int mvp_id = 0;

int load( char *buffer, int buffer_size, const char *filename )
{
	FILE *f = fopen( filename, "rb" );
	fseek( f, 0, SEEK_END );
	int size = ftell( f );
	if ( size > buffer_size ) size = buffer_size;
	fseek( f, 0, SEEK_SET );
	fread( buffer, 1, size, f );
	buffer[size] = 0;
	fclose( f );
	return size;
}

void init_OpenGL()
{
	printf( "%s\n", glGetString( GL_VERSION ) );

	const int buffer_size = 4096 * 4;
	const char *buffer = (char*)malloc( buffer_size );

	int compile_status = 0;
	int link_status = 0;

	//vertex shader
	unsigned int vertex_shader_id = glCreateShader( GL_VERTEX_SHADER );
	load( (char*)buffer, buffer_size, "vertex.glsl" );
	glShaderSource( vertex_shader_id, 1, &buffer, NULL );
	glCompileShader( vertex_shader_id );
	glGetShaderiv( vertex_shader_id, GL_COMPILE_STATUS, &compile_status );
	if ( compile_status == 0 )
	{
		static char info[1024];
		glGetShaderInfoLog( vertex_shader_id, 1024, NULL, info );
		printf( "vertex shader compiler error: %s\n", info );
	}

	//fragment shader
	unsigned int fragment_shader_id = glCreateShader( GL_FRAGMENT_SHADER );
	load( (char*)buffer, buffer_size, "fragment_color_change.glsl" );
	glShaderSource( fragment_shader_id, 1, &buffer, NULL );
	glCompileShader( fragment_shader_id );
	glGetShaderiv( fragment_shader_id, GL_COMPILE_STATUS, &compile_status );
	if ( compile_status == 0 )
	{
		static char info[1024];
		glGetShaderInfoLog( fragment_shader_id, 1024, NULL, info );
		printf( "fragment shader compiler error: %s\n", info );
	}

	//link shaders
	int program_id = glCreateProgram();
	glAttachShader( program_id, vertex_shader_id );
	glAttachShader( program_id, fragment_shader_id );
	glLinkProgram( program_id );
	glGetProgramiv( program_id, GL_LINK_STATUS, &link_status );
	if ( link_status == 0 )
	{
		static char info[1024];
		glGetProgramInfoLog( program_id, 1024, NULL, info );
		printf( "program linking error: %s\n", info );
	}

	glEnable( GL_DEPTH_TEST );

	glUseProgram( program_id );

	//delete shader objects
	glDeleteShader( vertex_shader_id );
	glDeleteShader( fragment_shader_id );

	float vertex_data[] =
	{
		//positions			//colors
		-0.5, -0.5, -0.5,	1.0, 0.0, 0.0,
		0.5, -0.5, -0.5,	1.0, 0.0, 0.0,
		0.5, 0.5, -0.5,		1.0, 0.0, 0.0,
		0.5, 0.5, -0.5,		1.0, 0.0, 0.0,
		-0.5, 0.5, -0.5,	1.0, 0.0, 0.0,
		-0.5, -0.5, -0.5,	1.0, 0.0, 0.0,

		-0.5, -0.5, 0.5,	0.0, 1.0, 0.0,
		0.5, -0.5, 0.5,		0.0, 1.0, 0.0,
		0.5, 0.5, 0.5,		0.0, 1.0, 0.0,
		0.5, 0.5, 0.5,		0.0, 1.0, 0.0,
		-0.5, 0.5, 0.5,		0.0, 1.0, 0.0,
		-0.5, -0.5, 0.5,	0.0, 1.0, 0.0,

		-0.5, 0.5, 0.5,		0.0, 0.0, 1.0,
		-0.5, 0.5, -0.5,	0.0, 0.0, 1.0,
		-0.5, -0.5, -0.5,	0.0, 0.0, 1.0,
		-0.5, -0.5, -0.5,	0.0, 0.0, 1.0,
		-0.5, -0.5, 0.5,	0.0, 0.0, 1.0,
		-0.5, 0.5, 0.5,		0.0, 0.0, 1.0,

		0.5, 0.5, 0.5,		1.0, 1.0, 0.0,
		0.5, 0.5, -0.5,		1.0, 1.0, 0.0,
		0.5, -0.5, -0.5,	1.0, 1.0, 0.0,
		0.5, -0.5, -0.5,	1.0, 1.0, 0.0,
		0.5, -0.5, 0.5,		1.0, 1.0, 0.0,
		0.5, 0.5, 0.5,		1.0, 1.0, 0.0,

		-0.5, -0.5, -0.5,	0.0, 1.0, 1.0,
		0.5, -0.5, -0.5,	0.0, 1.0, 1.0,
		0.5, -0.5, 0.5,		0.0, 1.0, 1.0,
		0.5, -0.5, 0.5,		0.0, 1.0, 1.0,
		-0.5, -0.5, 0.5,	0.0, 1.0, 1.0,
		-0.5, -0.5, -0.5,	0.0, 1.0, 1.0,

		-0.5, 0.5, -0.5,	1.0, 0.0, 1.0,
		0.5, 0.5, -0.5,		1.0, 0.0, 1.0,
		0.5, 0.5, 0.5,		1.0, 0.0, 1.0,
		0.5, 0.5, 0.5,		1.0, 0.0, 1.0,
		-0.5, 0.5, 0.5,		1.0, 0.0, 1.0,
		-0.5, 0.5, -0.5,	1.0, 0.0, 1.0
	};

	glGenVertexArrays( 1, &vertex_array_object );

	glGenBuffers( 1, &vertex_buffer_object );

	glBindVertexArray( vertex_array_object );
	glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer_object );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertex_data ), vertex_data, GL_STATIC_DRAW );

	//position attribute
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), NULL );
	glEnableVertexAttribArray( 0 );

	//color attribute
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)(3 * sizeof( float )) );
	glEnableVertexAttribArray( 1 );

	mvp_id = glGetUniformLocation( program_id, "mvp" );
}

void init()
{
	SDL_Init( SDL_INIT_VIDEO );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	window = SDL_CreateWindow( "Rotating Cube!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	SDL_GLContext context = SDL_GL_CreateContext( window );

	glewExperimental = GL_TRUE;
	glewInit();

	init_OpenGL();
}

int main( int argc, char **argv )
{
	init();

	unsigned char prev_key_state[256];
	unsigned char *keys = (unsigned char*)SDL_GetKeyboardState( NULL );

	bool done = false;
	while ( !done )
	{
		memcpy( prev_key_state, keys, 256 );

		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
			{
				done = true;
			}
		}

		int cmd_SPACE = 0;
		if ( keys[SDL_SCANCODE_SPACE] ) cmd_SPACE = 1;

		if ( cmd_SPACE == 1 )
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		else
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		
		//transformations
		glm::mat4 model = glm::mat4( 1.0f );
		glm::mat4 view = glm::mat4( 1.0f );
		glm::mat4 projection = glm::mat4( 1.0f );
		glm::mat4 mvp = glm::mat4( 1.0f );
		model = glm::rotate( model, (float)SDL_GetTicks() / 1000, glm::vec3( 0.5f, 1.0f, 0.0f ) );
		view = glm::translate( view, glm::vec3( 0.0f, 0.0f, -3.0f ) );
		projection = glm::perspective( glm::radians( 45.0f ), (float)screen_width / (float)screen_height, 0.1f, 100.0f );
		mvp = projection * view * model;
		glUniformMatrix4fv( mvp_id, 1, GL_FALSE, glm::value_ptr( mvp ) );

		//render
		glClearColor( 0.5, 0.5, 0.5, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glBindVertexArray( vertex_array_object );
		glDrawArrays( GL_TRIANGLES, 0, 36 );

		SDL_GL_SwapWindow( window );
	}

	return 0;
}