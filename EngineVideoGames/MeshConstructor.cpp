#define GLEW_STATIC
#include <GL\glew.h>
#include <queue>
#include "MeshConstructor.h"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "bezier2D.h"
#include "obj_loader.h"
#include <iostream>


MeshConstructor::MeshConstructor(const int type)
{
	

	switch (type)
	{
	case Axis:	
		InitLine(AxisGenerator());
		break;
	case Cube:
		 InitMesh(CubeTriangles());
		 break;
	case Octahedron:
		 InitMesh(OctahedronGenerator());
		 break;
	case Tethrahedron:
		 InitMesh(TethrahedronGenerator());
		 break;
	default:
		break;
	}

	
}

MeshConstructor::MeshConstructor(const std::string& fileName)
{
	InitMesh(OBJModel(fileName).ToIndexedModel());
}

bool MeshConstructor::checkCollision(BVH* other, glm::mat4 self_trans, glm::vec3 self_scale,
												 glm::mat4 other_trans, glm::vec3 other_scale)

{
	std::queue<BVH*> self_queue;
	BVH* self_curr=&this->bvh;
	self_queue.push(self_curr);
	int counter = 0;
	while (!self_queue.empty()) {
		counter++;
		self_curr = self_queue.front();
		self_queue.pop();
		other->box->updateDynamic(other_trans,other_scale);
		self_curr->box->updateDynamic(self_trans,self_scale);
		if (self_curr->box->checkCollision(other->box)) 

		{
			if(counter>=10)
				return self_curr->box->pickShape;
			if (self_curr->left != nullptr && self_curr->right != nullptr) {
				self_queue.push(self_curr->left);
				self_queue.push(self_curr->right);
			}
			else if (self_curr->left != nullptr)
				self_queue.push(self_curr->left);
			else if (self_curr->right != nullptr)
				self_queue.push(self_curr->right);
			else
			{
				//std::cout << self_curr->level << std::endl;
				//return self_curr->box->pickShape;
				return true;
			}
		}
		
	}

	//return -1;
	return false;
}

MeshConstructor::MeshConstructor(Bezier1D *curve,bool isSurface,unsigned int resT,unsigned int resS)
{
	if(isSurface)
	{
		Bezier2D surface(curve,glm::vec3(1,0,0),4);
		InitMesh(surface.GetSurface(resT,resS));		
	}
	else
	{
		InitLine( curve->GetLine(resT));
	}
}

MeshConstructor::MeshConstructor(const MeshConstructor &mesh)
{
	indicesNum = mesh.indicesNum;
	if(mesh.is2D)
		CopyMesh(mesh);
	else
		CopyLine(mesh);
}

MeshConstructor::~MeshConstructor(void)
{
	if(ib)
		delete ib;
	for (unsigned int i = 0; i < vbs.size(); i++)
	{
		if(vbs[i])
			delete vbs[i];
	}
}

void MeshConstructor::make_tree(std::vector<glm::vec3> positions)
{
	std::list<Node::vecType> point_list;
	for (auto i = 0; i < positions.size(); i++)
	{

		point_list.push_back(Node::vecType(positions[i].x, positions[i].y, positions[i].z, 1.0f));
	}
	tree.makeTree(point_list);
	this->bvh = *make_BVH(*tree.getRoot(), positions, 0);
	////tree.printTree(tree.getRoot());

	//BoundingBox *base = new BoundingBox(positions);

	//this->bvh.box = base;
	//this->bvh.left= make_BVH(*this->tree.getRoot(), *base, true, 0);
	//this->bvh.right=make_BVH(*this->tree.getRoot(), *base, false, 0);
}
// old bvh function
BVH* MeshConstructor::make_BVH (Node node, BoundingBox daddy, bool is_left, int level)
{
	int axis = level % 3;
	glm::vec3 center = daddy.static_center;
	glm::vec3 size = daddy.static_size;
	BVH *bvh = new BVH();

	
	if (is_left)
	{
		center[axis] = ((daddy.static_center[axis] + daddy.size[axis]) + node.data[axis])/2.0f;
		size[axis] = abs((daddy.static_center[axis] + daddy.size[axis]) - center[axis]);
	}
	else
	{
		center[axis] = ((daddy.static_center[axis] - daddy.size[axis]) + node.data[axis]) / 2.0f;
		size[axis] = abs((daddy.static_center[axis] - daddy.size[axis]) - center[axis]);
	}

	bvh->box = new BoundingBox(center, size) ;
	if (node.left != nullptr)
	{
		bvh->left=make_BVH(*(node.left), *bvh->box, true, level + 1);
	}
	if (node.right != nullptr)
	{
		bvh->right= make_BVH(*(node.right), *bvh->box, false, level + 1);
	}
	return bvh;
}


BVH* MeshConstructor::make_BVH(Node node, std::vector<glm::vec3> point_list, int level)
{
	int axis = level % 3;

	// TODO make distractor 
	BVH* bvh = new BVH();
	bvh->box = new BoundingBox(point_list);
	bvh->level = level;

	std::vector<glm::vec3> new_point_list;
	if (node.left != nullptr && float(point_list.size()/verticesNum) >= MINIMUM_VERTCIES_RATIO_FOR_BVH)
	{
		for (int i = 0; i < point_list.size(); i++)
		{
			if (node.data[axis] >= point_list[i][axis])
			{
				new_point_list.push_back(point_list[i]);
			}
		}
		bvh->left = make_BVH(*node.left, new_point_list, level + 1);

	}
	else
	{
		bvh->left = nullptr;
	}
	new_point_list.clear();

	if (node.right != nullptr && float(point_list.size() / verticesNum) >= MINIMUM_VERTCIES_RATIO_FOR_BVH)
	{
		for (int i = 0; i < point_list.size(); i++)
		{
			if (node.data[axis] < point_list[i][axis])
			{
				new_point_list.push_back(point_list[i]);
			}
		}
		bvh->right = make_BVH(*node.right, new_point_list, level + 1);

	}
	else
	{
		bvh->right = nullptr;
	}
	return bvh;
}

void MeshConstructor::InitLine(IndexedModel &model){
	
	verticesNum = model.positions.size();
	indicesNum = model.indices.size();
	
	vao.Bind();

	for (int i = 0; i < 2; i++)
	{
		vbs.push_back(new VertexBuffer(model.GetData(i),verticesNum*sizeof(model.positions[0])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	
	ib = new IndexBuffer((unsigned int*)model.GetData(5),indicesNum);
	
	vao.Unbind();
	is2D = false;
	
}

void MeshConstructor::InitMesh( IndexedModel &model){
	
	verticesNum = model.positions.size();
	indicesNum = model.indices.size();
	make_tree(model.positions);
	vao.Bind();

	for (int i = 0; i < 4; i++)
	{
		vbs.push_back(new VertexBuffer(model.GetData(i),verticesNum*sizeof(model.positions[0])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	vbs.push_back(new VertexBuffer(model.GetData(4),verticesNum*sizeof(model.texCoords[0])));
	vao.AddBuffer(*vbs.back(),vbs.size()-1,2,GL_FLOAT);


	
	ib = new IndexBuffer((unsigned int*)model.GetData(5),indicesNum);
	
	vao.Unbind();
	is2D = true;
	
}

void MeshConstructor::CopyLine(const MeshConstructor &mesh){
	
	vao.Bind();

	for (int i = 0; i < 2; i++)
	{
		vbs.push_back(new VertexBuffer(*(mesh.vbs[i])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	
	ib = new IndexBuffer(*mesh.ib);
	
	vao.Unbind();

	is2D = false;
	
}

void MeshConstructor::CopyMesh(const MeshConstructor &mesh){
	tree = *new Kdtree(mesh.tree);
	vao.Bind();

	for (int i = 0; i < 4; i++)
	{
		vbs.push_back(new VertexBuffer(*(mesh.vbs[i])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	
	
	ib = new IndexBuffer(*mesh.ib);
	//ib = mesh.ib;
	vao.Unbind();

	is2D = true;
	
}

BVH::BVH()
{
	this->box = new BoundingBox(glm::vec3(0), glm::vec3(0));
	this->left = nullptr;
	this->right = nullptr;

}

BVH::BVH(BVH & bvh_other)
{
	this->box = new BoundingBox(bvh_other.box->static_center, bvh_other.box->size);
	if (bvh_other.left != nullptr)
	{
		this->left = new BVH(*bvh_other.left);
	}
	else
	{
		this->left = nullptr;
	}
	if (bvh_other.right != nullptr)
	{
		this->right = new BVH(*bvh_other.right);
	}
	else
	{
		this->right = nullptr;
	}
}
