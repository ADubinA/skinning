#pragma once
#include "bezier1D.h"
#include "scene.h"
class Snak



{
private:
	int head_indx;
	
	int num_of_links;
	glm::vec3 velocity;
	bool is_dead;
	Scene* scn;
	
public:
	int tail_indx;
	enum Direction
	{
		Up,
		Down,
		Left,
		Right,
		Forward
	};
	void createSnake();
	void move(Direction direction);
	glm::vec3 get_head_pos();

	Snak(int num_of_joints, Scene* scn);
	~Snak();
};

