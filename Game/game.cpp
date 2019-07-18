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
	pickedShape = this->shapes.size()-1;
	shapeTransformation(yGlobalTranslate, 5);
	shapes[pickedShape]->SetTexture(sun);
	shapeTransformation(yScale, 0.05);
	shapeTransformation(xScale, 0.05);
	shapeTransformation(zScale, 0.05);

	//addShapeCopy(pickedShape, -1, TRIANGLES);
	/*pickedShape = this->shapes.size() - 1;
	shapes[pickedShape]->AddTexture(textures[1]);

	shapeTransformation(zGlobalTranslate, -5);
	shapeTransformation(yScale, 0.05);
	shapeTransformation(xScale, 0.05);
	shapeTransformation(zScale, 0.05);*/
	//for (int i = 0; i<this->shapes.size(); i++)
	//{
	//	Shape *shape = shapes[i];
	//	if (shape->mode >= TRIANGLES)
	//	{
	//		BVH *bvh = &shape->mesh->bvh;

	//		CreateBoundingBoxes(bvh, i, 0);


	//	}
	//}

	ReadPixel();
	pickedShape = -1;
	Activate();
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
	if(isActive)
	{
		int p = pickedShape;
		this->snak->move(snak->Forward);
		pickedShape = p;
	}
}

Game::~Game(void)
{
	delete curve;
}
