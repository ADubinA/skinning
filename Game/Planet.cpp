#include "Planet.h"



Planet::Planet(float self_rotation_speed, float parent_rotation_speed,int shape_index,float planet_size,int texture_index,glm::vec3 starting_pos):Shape(OBJ_FILE_LOCATION, TRIANGLES)
{
	this->self_rotation_speed = self_rotation_speed;
	this->parent_rotation_speed = parent_rotation_speed;
	this->shape_index = shape_index;
	this->planet_size = planet_size;
	this->texture_index = texture_index;
	this->starting_pos = starting_pos;
	build_planet();
}

void Planet:: build_planet()
{
	// 1 means local translate
	
	this->myTranslate(glm::vec3(this->starting_pos), 1);
	this->myScale(glm::vec3(this->planet_size, this->planet_size, this->planet_size));
	this->SetTexture(this->texture_index);
}

void Planet::move()
{
	this->myRotate(this->parent_rotation_speed,ROTATION_AXIS, -1);
}


Planet::~Planet()
{
}
