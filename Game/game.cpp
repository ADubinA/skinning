#include "game.h"
#include <iostream>
#include <list>
#include <math.h>

#define BOUNDING_BOX_INDEX 2


static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}



Game::Game():Scene(){curve = 0;}

Game::Game(glm::vec3 position,float angle,float hwRelation,float near, float far) : Scene(position,angle,hwRelation,near,far)
{ 
	curve = new Bezier1D();
	this->isStarted = false;
}

void Game::addShape(int type,int parent,unsigned int mode)
{
		chainParents.push_back(parent);
		if(type!=BezierLine && type!=BezierSurface)
			shapes.push_back(new Shape(type,mode));

}
void Game::addShape(Bezier1D * curve, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(curve, 30, 30, true, mode));

}

void Game::addSolarSystem()
{
	//float self_rotation_speed, float parent_rotation_speed,int shape_index,
	//float planet_size,int texture_index,glm::vec3 starting_pos
	addPlanet(0.2f,0.2f, shapes.size(),0.2f,sun, glm::vec3 (0,0,-20),-1,8);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.015f, mercury, glm::vec3(10, 0, 0), planets[8], 7);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.018, mars, glm::vec3(-13, 0, 0), planets[8], 6);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.02f, earth, glm::vec3(0, 0, 17), planets[8], 5);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.015f, venus, glm::vec3(0, 0, -18), planets[8], 4);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.08f, jupiter, glm::vec3(21, 0, 0), planets[8], 3);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.07f, saturn, glm::vec3(0, 0, -22), planets[8], 2);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.05f, neptune, glm::vec3(-24, 0, 0), planets[8], 1);

	addPlanet(0.2f, 0.2f, shapes.size(), 0.04f, uranus, glm::vec3(0, 0, 27), planets[8], 0);



}

void Game::addPlanet(float self_rotation_speed, float parent_rotation_speed, int shape_index, float planet_size, int texture_index, glm::vec3 starting_pos, int parent, int indx)
{
	chainParents.push_back(parent);
	Planet *planet = new Planet(self_rotation_speed, parent_rotation_speed, shape_index, planet_size, texture_index, starting_pos);
	this->planets[indx] = planet->shape_index;
	this->shapes.push_back(planet);
}



void Game::CreateBoundingBoxes(BVH * box_tree, int parent, int level)
{
	this->addShapeCopy(BOUNDING_BOX_INDEX, -1, LINE_LOOP);
	
	pickedShape = shapes.size() - 1;
	box_tree->box->setPickShape(pickedShape);
	box_tree->level = level;
	shapes[pickedShape]->Hide();
	shapes[pickedShape]->shaderID = BASIC_SHADER;

	shapeTransformation(xLocalTranslate, box_tree->box->static_center.x);
	shapeTransformation(yLocalTranslate, box_tree->box->static_center.y);
	shapeTransformation(zLocalTranslate, box_tree->box->static_center.z);


	shapeTransformation(xScale, box_tree->box->static_size.x);
	shapeTransformation(yScale, box_tree->box->static_size.y);
	shapeTransformation(zScale, box_tree->box->static_size.z);

	chainParents[pickedShape] = parent;


	if (level < 1 )
	{
		shapes[pickedShape]->Unhide();
	}
	if (box_tree->left != nullptr)
	{
		CreateBoundingBoxes(box_tree->left, parent, level+1);
	}
	if (box_tree->right != nullptr)
	{
		CreateBoundingBoxes(box_tree->right, parent, level+1);
	}
}


void Game::Init()
{
	playTune("Sounds/Jump.wav");
	this->AddTexture("../res/textures/metal_tread.jpg");
	this->AddTexture("../res/textures/earth.jpg");
	this->AddTexture("../res/textures/jupiter.jpg");
	this->AddTexture("../res/textures/mars.jpg");
	this->AddTexture("../res/textures/mercury.jpg");
	this->AddTexture("../res/textures/neptune.jpg");
	this->AddTexture("../res/textures/saturn.jpg");
	this->AddTexture("../res/textures/uranus.jpg");
	this->AddTexture("../res/textures/venus.jpg");
	this->AddTexture("../res/textures/sun.jpg");
	this->AddTexture("../res/textures/galaxy2.jpg");

	std::vector<TransStruct> data;
	addShape(Axis, -1, LINES);
	
	//createSnake(3);
	this->snak = new Snak(3, this);
	this->snak->createSnake();

	//translate all scene away from camera
	//myTranslate(glm::vec3(0, 0, -10), 0);

	pickedShape = 0;

	shapeTransformation(yScale, 10);
	shapeTransformation(xScale, 10);
	shapeTransformation(zScale, 10);

	addShapeFromFile("../res/objs/sphere.obj", -1, TRIANGLES);
	pickedShape = this->shapes.size() - 1;
	this->spacse_indx = pickedShape;
	shapeTransformation(yScale, 8);
	shapeTransformation(xScale, 8);
	shapeTransformation(zScale, 8);
	shapeTransformation(zGlobalTranslate, -10);
	shapes[pickedShape]->SetTexture(galaxy);

	addSolarSystem();
	
	ReadPixel();
	pickedShape = -1;
	//Activate();
	// add camera for the head (this is first person)
	cameras.push_back(new Camera(glm::vec3(0.0f, 0.0f, 10.0f)));
	cameras.push_back(new Camera(snak->get_head_pos()));
	cameras[FirstPersonCamera]->RotateCamera(glm::rotate(90.0f, glm::vec3(0, 1, 0)), snak->get_head_pos());
	this->cameraIndx = ThirdPersonCamera;

}

void Game::Update(const glm::mat4 &MV, const glm::mat4 &P, const glm::mat4 &Normal, int indx, Shader *s, int s_indx)
{
	s->Bind();
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;

	switch (s_indx)
	{
	case BASIC_SHADER:
		s->SetUniformMat4f("MVP", P*MV);

		break;
	case SKINNING_SHADER:
		s->SetUniformMat4f("MV", MV);
		s->SetUniformMat4f("P", P);
		if (snak->tail_indx != indx)
		{
			s->SetUniform3i("jointIndex", indx - 1, indx, indx + 1);
		}
		else
		{
			s->SetUniform3i("jointIndex", indx - 1, indx, indx);
		}
		break;
	default:
		break;
	}
	s->SetUniformMat4f("Normal", Normal);
	s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 1.0f);
	s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);	
	
}

void Game::WhenRotate()
{
	//if(pickedShape>=0)
	//	printMat(GetShapeTransformation());
}

void Game::WhenTranslate()
{
	if(pickedShape>=0)
	{
		glm::vec4 pos = GetShapeTransformation()*glm::vec4(0,0,0,1);
	//	std::cout<<"( "<<pos.x<<", "<<pos.y<<", "<<pos.z<<")"<<std::endl;
	}
}

void Game::Motion()
{

	collisionDetection();
	snak->align_cameras();
	if (glm::distance(this->snak->get_head_pos(), glm::vec3(0, 0, 0)) > 100)
		this->Deactivate();
	if(isActive)
	{
		int p = pickedShape;
		this->snak->move(snak->Forward);
		pickedShape = p;
	}
	((Planet*)(shapes[planets[8]]))->move();

}

Game::~Game(void)
{
	delete curve;
}
