#pragma once
#include "shape.h"
#define OBJ_FILE_LOCATION "../res/objs/sphere.obj"
#define TRIANGLES 4
#define ROTATION_AXIS glm::vec3(0,1,0)
#define metal_tread 0
#define earth 1
#define jupiter 2
#define mars 3
#define mercury 4
#define neptune 5
#define saturn 6
#define uranus 7
#define venus 8
#define sun 9
#define galaxy 10
class Planet : public Shape
{
public:
	Planet(float self_rotation_speed, float parent_rotation_speed, int shape_index, float planet_size, int texture_index, glm::vec3 starting_pos) ;
	void build_planet();
	void move();
	int shape_index;
	~Planet();
private:
	float self_rotation_speed;
	float parent_rotation_speed;
	
	float planet_size;
	int texture_index;
	glm::vec3 starting_pos;
	

};

