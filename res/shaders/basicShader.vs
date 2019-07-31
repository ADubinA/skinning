#version 330

attribute vec3 position;
attribute vec3 color;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec2 texCoord0;
varying vec3 normal0;
varying vec3 color0;
varying vec3 sunlight_dir;

uniform mat4 MV;
uniform mat4 P;
uniform mat4 Normal;



void main()
{
	vec3 sun_position = vec3(0,0,-20);
	if(length(sun_position - position)<0.00001)
	{
		sunlight_dir = vec3(0,0,1);
	}
	else
	{
		sunlight_dir = normalize(sun_position -(MV*(vec4( position,1.0f))).xyz);
	}
	texCoord0 = texCoords;
	color0 = color;
	normal0 = normalize((Normal * vec4(normal, 0.0)).xyz);
	gl_Position =P*MV * vec4(position, 1.0);
}
