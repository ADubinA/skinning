#pragma once
#include "shape.h"
#define OBJ_FILE_LOCATION "../res/objs/sphere.obj"
#define TRIANGLES 4
#define ROTATION_AXIS glm::vec3(0,1,0)
class Planet : public Shape
{
public:
	Planet(float self_rotation_speed, float parent_rotation_speed, int shape_index, float planet_size, int texture_index, glm::vec3 starting_pos) ;
	void build_planet();
	void move();
	~Planet();
private:
	float self_rotation_speed;
	float parent_rotation_speed;
	int shape_index;
	float planet_size;
	int texture_index;
	glm::vec3 starting_pos;
	

};

