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


attribute vec3 weights;

uniform ivec3 jointIndex;
uniform vec4 Qrot[30];
uniform vec4 Qtrans[30];



void main (void) 
{
	// sun light
	vec3 sun_position = vec3(0,0,-20);
	if(length(sun_position - position)<0.00001)
	{
		sunlight_dir = vec3(0,0,1);
	}
	else
	{
		sunlight_dir = normalize(sun_position -(MV*(vec4( position,1.0f))).xyz);
	}
if(jointIndex.y>0)
{
  vec4 b_0 = Qrot[jointIndex.x] * weights.x +
			 Qrot[jointIndex.y] * weights.y +
			 Qrot[jointIndex.z] * weights.z;

  vec4 b_E = Qtrans[jointIndex.x] * weights.x +
			 Qtrans[jointIndex.y] * weights.y +
			 Qtrans[jointIndex.z] * weights.z;

  // normalize b_0 and b_E
  float b_norm = length(b_0);
  vec4 c_0 = b_0 / b_norm;
  vec4 c_E = b_E / b_norm;
  
   
  float x_0 = c_0[1];
  float y_0 = c_0[2];
  float z_0 = c_0[3];
  float w_0 = c_0[0];
  float x_E = c_E[1];
  float y_E = c_E[2];
  float z_E = c_E[3];
  float w_E = c_E[0];


  float t0 = 2.0 * (-w_E * x_0 + x_E * w_0 - y_E * z_0 + z_E * y_0);
  float t1 = 2.0 * (-w_E * y_0 + x_E * z_0 + y_E * w_0 - z_E * x_0);
  float t2 = 2.0 * (-w_E * z_0 - x_E * y_0 + y_E * x_0 + z_E * w_0);
  
  mat4 Qmat = mat4(
        1.0 - (2.0 * y_0 * y_0) - (2.0 * z_0 * z_0),
		(2.0 * x_0 * y_0) + (2.0 * w_0 * z_0),
        (2.0 * x_0 * z_0) - (2.0 * w_0 * y_0),
        0,

        (2.0 * x_0 * y_0) - (2.0 * w_0 * z_0),
        1.0 - (2.0 * x_0 * x_0) - (2.0 * z_0 * z_0),
        (2.0 * y_0 * z_0) + (2.0 * w_0 * x_0),
        0,

        (2.0 * x_0 * z_0) + (2.0 * w_0 * y_0),
        (2.0 * y_0 * z_0) - (2.0 * w_0 * x_0),
        1.0 - (2.0 * x_0 * x_0) - (2.0 * y_0 * y_0),
        0,

        t0, t1, t2, 1);
		
  texCoord0 = texCoords;
  color0 = weights;
  
  normal0 = ( Qmat * vec4(normal, 0.0)).xyz;
  gl_Position =  P  *Qmat * vec4(position.x+jointIndex.y*0.9-3,position.y,position.z, 1.0);
  }
  else
  {
	normal0 = (Normal * vec4(normal, 0.0)).xyz;
	gl_Position =P *MV* vec4(position, 1.0);
  }
}
