#pragma once
#include "scene.h"
#include "bezier1D.h"
#include "snak.h"
#include "planet.h"

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

class Game : public Scene
{
	struct TransStruct
	{
		int level;
		glm::vec4 data;
		glm::vec3 translate;
		glm::vec3 resize;
		int picked_shape;

	};
	Bezier1D *curve;
	void Game::CreateBoundingBoxes(BVH *box_tree, int parent, int level);
	std::vector<Planet*> planets;

public:
	Snak* snak;
	Game(void);
	Game(glm::vec3 position,float angle,float hwRelation,float near, float far);
	void Init();
	void addShape(int type,int parent,unsigned int mode);
	void addShape(Bezier1D * curve, int parent, unsigned int mode);
	void addSolarSystem();
	void addPlanet(float self_rotation_speed, float parent_rotation_speed, int shape_index, float planet_size, int texture_index, glm::vec3 starting_pos);
//	void Update( glm::mat4 MVP ,glm::mat4 *jointTransforms,const int length,const int  shaderIndx);
	void Update(const glm::mat4 &MV, const glm::mat4 &P ,const glm::mat4 &Normal, int indx, Shader *s, int s_index);
	void ControlPointUpdate();
	void WhenRotate();
	void WhenTranslate();
	void Motion();
	~Game(void);
};
