#define GLEW_STATIC
#include <GL\glew.h>
#include "shape.h"
#include "Log.hpp"
#include <iostream>

Shape::Shape(const Shape& shape,unsigned int mode)
{
	
	mesh = new MeshConstructor(*shape.mesh);
	mesh->bvh =*new BVH (shape.mesh->bvh);
	tex = shape.tex;
	isCopy = true;
	this->mode = mode;
	toRender = true;
	this->shader_indx = shape.shader_indx;
	
}

Shape::Shape(const std::string& fileName, unsigned int mode){
	mesh = new MeshConstructor(fileName);
	isCopy = false;
	this->mode = mode;
	toRender = true;
	this->shader_indx = Basic;
}

Shape::Shape(const int SimpleShapeType,unsigned int mode)
{
	mesh = new MeshConstructor(SimpleShapeType);
	//mesh->Bind();
	this->mode = mode;
	isCopy = false;
	toRender = true;
	this->shader_indx = Basic;
}

Shape::Shape(Bezier1D *curve, unsigned int xResolution,unsigned int yResolution,bool is2D,unsigned int mode)
{
	mesh = new MeshConstructor(curve,is2D,xResolution,yResolution);
	this->mode = mode;
	isCopy = false;
	toRender = true;
	this->shader_indx = Basic;
}


void Shape::AddTexture(const std::string& textureFileName)
{
	tex = new Texture(textureFileName);
}

void Shape::AddTexture(Texture *tex )
{
	this->tex = tex;
}
bool Shape::checkCollision(Shape * other,glm::mat4 this_trans, glm::mat4 other_trans)
{
	std::queue<BVH*> other_queue;
	BVH* other_curr ;
	other_queue.push(&other->mesh->bvh);
	bool picked = false;
	int counter = 0;

	while (!other_queue.empty()) 
	{
		
		counter++;
		other_curr = other_queue.front();
		other_queue.pop();

		picked = this->mesh->checkCollision(other_curr,this_trans,this->getScale(),other_trans,other->getScale());

		if (picked)

		{
			/*if (counter >= 10)
				return picked;*/
			if (other_curr->left != nullptr && other_curr->right != nullptr) {
				other_queue.push(other_curr->left);
				other_queue.push(other_curr->right);
			}
			else if (other_curr->left != nullptr)
				other_queue.push(other_curr->left);
			else if (other_curr->right != nullptr)
				other_queue.push(other_curr->right);
			else
			{
				//std::cout << other_curr->level << std::endl;

				return true;
			}
		}
	}
	return false;

}


void Shape::Draw( const Shader& shader)
{
	if(tex)
		tex->Bind();

	shader.Bind();
	mesh->Bind();
	/*if(isCopy)
		glDrawArrays(GL_TRIANGLES, 0, indicesNum);
	else*/
	GLCall(glDrawElements(mode,mesh->GetIndicesNum(), GL_UNSIGNED_INT, 0));
	mesh->Unbind();
}

Shape::~Shape(void)
{
	if(!isCopy)
	{
		if(mesh)
			delete mesh;
		//if(tex)
			//delete tex;
	}
}

