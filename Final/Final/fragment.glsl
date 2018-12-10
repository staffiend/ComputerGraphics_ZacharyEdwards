#version 330 core

out vec4 fragment_color;

struct Material
{
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 fragment_pos;
in vec3 normal;
in vec2 texcoords;

uniform vec3 view_pos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * texture(material.diffuse, texcoords).rgb;
	
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light.position - fragment_pos);
	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texcoords).rgb;
	
	vec3 view_direction = normalize(view_pos - fragment_pos);
	vec3 reflect_direction = reflect(light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);
	
	vec3 result = ambient + diffuse + specular;
	fragment_color = vec4(result, 1.0);
}