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

void Game::createSnake(int num_of_joints)
{
	Bezier1D * body = new Bezier1D(BODY);
	Bezier1D * edge = new Bezier1D(EDGE);
	addShape(body, -1, 5);
	int body_index = shapes.size() - 1;
	shapes[shapes.size() - 1]->Hide();

	addShape(edge, -1, 5);
	int edge_index = shapes.size() - 1;

	
	

	BoundingBox* bodybox = shapes[body_index]->mesh->bvh.box;
	BoundingBox* edgebox = shapes[edge_index]->mesh->bvh.box;
	pickedShape = edge_index;
	shapeTransformation(xGlobalTranslate, edgebox->static_center.x - bodybox->size.x);
	shapeTransformation(yGlobalTranslate, edgebox->static_center.y);
	shapeTransformation(zGlobalTranslate, edgebox->static_center.z);
	for (int i = 0; i < num_of_joints; i++) {
		//addShapeCopy(body_index, edge_index + i - 1, QUADS);
		addShapeCopy(body_index,- 1, 5);
		pickedShape = shapes.size() - 1;
		shapeTransformation(xGlobalTranslate, bodybox->static_center.x + bodybox->size.x	 );
		shapeTransformation(yGlobalTranslate, bodybox->static_center.y 	);
		shapeTransformation(zGlobalTranslate, bodybox->static_center.z 	);
		chainParents[pickedShape] = pickedShape-1;
	}


	addShapeCopy(edge_index, - 1, 5);
	pickedShape = shapes.size() - 1;
	shapeTransformation(zLocalRotate, 180);
	shapeTransformation(xGlobalTranslate, edgebox->static_center.x +  bodybox->size.x);
	shapeTransformation(xLocalTranslate,  -2*bodybox->size.x);
	shapeTransformation(yGlobalTranslate, edgebox->static_center.y);
	shapeTransformation(zGlobalTranslate, edgebox->static_center.z);
	chainParents[pickedShape] = pickedShape - 1;


}

void Game::CreateBoundingBoxes(BVH * box_tree, int parent, int level)
{
	this->addShapeCopy(BOUNDING_BOX_INDEX, -1, LINE_LOOP);
	pickedShape = shapes.size() - 1;
	box_tree->box->setPickShape(pickedShape);
	box_tree->level = level;
	shapes[pickedShape]->Hide();

	shapeTransformation(xLocalTranslate, box_tree->box->static_center.x);
	shapeTransformation(yLocalTranslate, box_tree->box->static_center.y);
	shapeTransformation(zLocalTranslate, box_tree->box->static_center.z);


	shapeTransformation(xScale, box_tree->box->size.x);
	shapeTransformation(yScale, box_tree->box->size.y);
	shapeTransformation(zScale, box_tree->box->size.z);

	chainParents[pickedShape] = parent;


	if (level ==0 )
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
	std::vector<TransStruct> data;
	addShape(Axis,-1,LINES);
	createSnake(3);

	//translate all scene away from camera
	myTranslate(glm::vec3(0, 0, -10), 0);

	pickedShape = 0;

	shapeTransformation(yScale, 10);
	shapeTransformation(xScale, 10);
	shapeTransformation(zScale, 10);

	ReadPixel();
	pickedShape = -1;
	Activate();

	//addShapeCopy(3,2,LINE_LOOP);+		vao	{m_RendererID=4 }	VertexArray

	// created bvh visual
	/*for (int i=0; i<this->shapes.size(); i++)
	{
		Shape *shape = shapes[i];
		if (shape->mode == TRIANGLES)
		{
			BVH *bvh = &shape->mesh->bvh;
			
			CreateBoundingBoxes(bvh, i, 0);


		}
	}*/
	
}

void Game::Update(const glm::mat4 &MV, const glm::mat4 &P, const glm::mat4 &Normal, int indx, Shader *s)
{
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MV", MV);
	s->SetUniformMat4f("P", P);
	s->SetUniform3i("jointIndex", indx-1,indx,indx+1); // TODO make a better function for this shit
	
	
	s->SetUniform3i("jointIndex", indx - 1, indx, indx + 1);
	//s->SetUniformMat4f("MVP", P*MV);
	s->SetUniformMat4f("Normal", Normal);
	s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 1.0f);
	s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);	
	//s->SetUniform3i("jointIndices", joint_indecies.x, joint_indecies.y, joint_indecies.z);
	collisionDetection();
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
	if(isActive)
	{
		int p = pickedShape;
		if (this->tmp_test_mode)
		{
			pickedShape = 1;
			shapeTransformation(zLocalRotate, 0.45);
			pickedShape = 3;
			shapeTransformation(yGlobalTranslate, -0.2);
		}
		pickedShape = p;
	}
}

Game::~Game(void)
{
	delete curve;
}
