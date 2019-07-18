#pragma once
#include "bezier1D.h"
#include "scene.h"
class Snak
#define SNAKE_TEX_INDEX 0
#define ROTATION_SPEED 5.0f
{
private:
	
	int num_of_links;
	glm::vec3 velocity;
	bool is_dead;
	Scene* scn;
	std::vector<int> index_chain; //shape indeces starting from head to tail
public:
	int head_indx;
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

