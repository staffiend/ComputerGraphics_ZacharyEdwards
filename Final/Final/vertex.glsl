#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 tex_coords;

out vec3 fragment_pos;
out vec3 normal;
out vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragment_pos = vec3(model * vec4(pos, 1.0));
	normal = mat3(transpose(inverse(model))) * normals;
	texcoords = tex_coords;
	gl_Position = projection * view * vec4(fragment_pos, 1.0);
}