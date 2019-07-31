#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 sunlight_dir;

uniform vec4 lightColor;
uniform sampler2D sampler;
uniform vec4 lightDirection;

uniform mat4 MV;
uniform mat4 P;
uniform mat4 Normal;

void main()
{
    vec3 tmp = dot(sunlight_dir, normal0)* vec3(1.0f,1.0f,1.0f) ;
	gl_FragColor = 0.5*texture2D(sampler, texCoord0)+texture2D(sampler, texCoord0)*clamp(vec4(tmp,1.0), 0.0, 1.0);
    //gl_FragColor = clamp(vec4(tmp,1),0.0,1.0);
}
