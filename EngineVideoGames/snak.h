#pragma once
#include "bezier1D.h"
#include "scene.h"
class Snak
#define SNAKE_TEX_INDEX 0
#define PI 3.14159265358979323846264338327950288
#define ROTATION_SPEED 5.0f
{
private:
	
	int num_of_links;
	glm::vec3 velocity;
	bool is_dead;
	Scene* scn;
	float interpolation_constant;
	std::vector<int> index_chain; //shape indeces starting from head to tail
public:
	int head_indx;
	int tail_indx;
	float speed;
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

	glm::vec3 get_segment_tip(int indx);

	glm::vec3 get_segment_center(int indx);

	void align_segments(int stationary, int moving);


	Snak(int num_of_joints, Scene* scn);
	~Snak();
};

