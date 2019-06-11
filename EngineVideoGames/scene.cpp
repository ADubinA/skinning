#define GLEW_STATIC
#include <GL\glew.h>
#include "scene.h"
#include <iostream>
#include <glm/gtc/quaternion.hpp>


using namespace std;
using namespace glm;



	static void printMat(const mat4 mat)
{
	printf(" matrix: \n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			printf("%f ", mat[j][i]);
		printf("\n");
	}
}
	Scene::Scene()
	{
		//verticesSize = sizeof(vertices)/sizeof(vertices[0]);
		//
		//indicesSize = sizeof(indices)/sizeof(indices[0]) ; 
		glLineWidth(3);
		
		cameras.push_back(new Camera(vec3(0,0,1.0f),60.0f,1.0f,0.1f,100.0f));
		pickedShape = -1;
		depth = 0;
		cameraIndx = 0;
		xold = 0;
		yold = 0;
		isActive = false;
	}

	Scene::Scene(vec3 position,float angle,float hwRelation,float near, float far)
	{
		//verticesSize = sizeof(vertices)/sizeof(vertices[0]);
		//
		//indicesSize = sizeof(indices)/sizeof(indices[0]) ; 
		glLineWidth(6);
		cameras.push_back(new Camera(position,angle,hwRelation,near,far));
	//	axisMesh = new Shape(axisVertices,sizeof(axisVertices)/sizeof(axisVertices[0]),axisIndices, sizeof(axisIndices)/sizeof(axisIndices[0]));
		pickedShape = -1;
		depth = 0;
		cameraIndx = 0;
		xold = 0;
		yold = 0;
		isActive = false;
	}

	void Scene::addShapeFromFile(const std::string& fileName,int parent,unsigned int mode)
	{
		chainParents.push_back(parent);
		shapes.push_back(new Shape(fileName,mode));
	}

	void Scene::addShape(int type, int parent,unsigned int mode)
	{
		chainParents.push_back(parent);
		shapes.push_back(new Shape(type,mode));
	}

	void Scene::addShape(Bezier1D * curve, int parent, unsigned int mode)
	{
	}

	void Scene::addShapeCopy(int indx,int parent,unsigned int mode)
	{
		chainParents.push_back(parent);
		shapes.push_back(new Shape(*shapes[indx],mode));
	}

	void Scene::collisionDetection()
	{
		int picked;
		for (auto& shape1 : shapes)
		{
			if (shape1->mode == TRIANGLES)
			{
				for (auto& shape2 : shapes)
				{
					// TODO check that equality is very true
					if (shape2->mode == TRIANGLES && &shape1!=&shape2)
					{
						picked = shape1->checkCollision(shape2);
						if (picked >= 0)
						{
							shapes[picked]->Unhide();
							this->tmp_test_mode = false;
						}
					}
				}
			}
		}
	}

	void Scene::addShader(const std::string& fileName)
	{
		shaders.push_back(new Shader(fileName));
		
	}

	mat4 Scene::GetViewProjection(int indx) const
	{
		return cameras[indx]->GetViewProjection();
	}

	mat4 Scene::GetShapeTransformation() const
	{
		return shapes[pickedShape]->makeTrans();
	}

	void Scene::Draw(int shaderIndx,int cameraIndx,bool debugMode)
	{
		glm::mat4 Normal = makeTrans();
		glm::mat4 MV =   Normal;
		glm::mat4 P = cameras[0]->GetViewProjection() ;
		glm::vec4 Qrot[NUM_OF_SHAPES];
		glm::vec4 Qtrans[NUM_OF_SHAPES];
		int p = pickedShape;
//		shaders[shaderIndx]->Bind();
		for (unsigned int i=0; i<shapes.size();i++)
		{
			if(shapes[i]->Is2Render())
			{
				mat4 Normal1 = mat4(1);
				pickedShape = i;
				for (int j = i; chainParents[j] > -1; j = chainParents[j])
				{
					Normal1 =  shapes[chainParents[j]]->makeTrans() * Normal1;
				}
			
				mat4 MV1 = MV * Normal1; 
				Normal1 = Normal * Normal1;

				MV1 = MV1 * shapes[i]->makeTransScale(mat4(1));
				Normal1 = Normal1 * shapes[i]->makeTrans();

				Update(MV1,P,Normal1 ,i,shaders[shaderIndx]);

				if(shaderIndx == 1)
					shapes[i]->Draw(*shaders[shaderIndx]);
				else 
					shapes[i]->Draw(*shaders[shaderIndx]);


				glm::mat3x4 small_tran = glm::mat3x4(transpose(MV1));
				glm::dualquat quatDul = glm::dualquat_cast(small_tran);


				Qrot[i]   = glm::vec4(	quatDul.real.w,
										quatDul.real.x,
										quatDul.real.y,
										quatDul.real.z);

				Qtrans[i] = glm::vec4(	quatDul.dual.w,
					quatDul.dual.x,
						quatDul.dual.y,
					quatDul.dual.z);

			}
		}

		shaders[shaderIndx]->SetUniform4v("Qrot", Qrot);
		shaders[shaderIndx]->SetUniform4v("Qtrans", Qtrans);

		pickedShape = p;
	}

	 void Scene::shapeRotation(vec3 v, float ang,int indx)
		{
			if(v.x >0.9999)
				shapes[indx]->myRotate(ang,v,xAxis1);
			else 
				if(v.z >0.9999)
					shapes[indx]->myRotate(ang,v,zAxis12);
			else
				shapes[indx]->myRotate(ang,v,-1);
		}

	void Scene::shapeTransformation(int type,float amt)
	{
		    vec3 newAxis;
			switch (type)
			{
			case xLocalTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(amt,0,0),1);
				else
				{
					int i = pickedShape;
					for (; chainParents[i] > -1; i = chainParents[i]);
					shapes[i]->myTranslate(vec3(amt,0,0),1);
				}
				break;
			case yLocalTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(0,amt,0),1);
				else
					{
						int i = pickedShape;
						for (; chainParents[i] > -1; i = chainParents[i]);
						shapes[i]->myTranslate(vec3(0,amt,0),1);
					}
				break;
			case zLocalTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(0,0,amt),1);
				else
				{
					int i = pickedShape;
					for (; chainParents[i] > -1; i = chainParents[i]);
					shapes[i]->myTranslate(vec3(0,0,amt),1);
				}
				break;
			case xGlobalTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(amt/5.0,0,0),0);
				else
				{
					int i = pickedShape;
					for (; chainParents[i] > -1; i = chainParents[i]);
					shapes[i]->myTranslate(vec3(amt,0,0),0);
				}
				break;
			case yGlobalTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(0,amt/5.0,0),0);
				else
				{
					int i = pickedShape;
					for (; chainParents[i] > -1; i = chainParents[i]);
					shapes[i]->myTranslate(vec3(0,amt,0),0);
				}
				break;
			case zGlobalTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(0,0,amt/5.0),0);
								else
				{
					int i = pickedShape;
					for (; chainParents[i] > -1; i = chainParents[i]);
					shapes[i]->myTranslate(vec3(0,0,amt),0);
				}
				break;
			case xLocalRotate:
				if(pickedShape ==-1)
					myRotate(amt,vec3(1,0,0),xAxis1);
				else
					shapes[pickedShape]->myRotate(amt,vec3(1,0,0),xAxis1);
				break;
			case yLocalRotate:
				if(pickedShape ==-1)
					myRotate(amt,vec3(0,1,0),-1);
				else
					shapes[pickedShape]->myRotate(amt,vec3(0,1,0),-1);
				break;
			case zLocalRotate:
				if(pickedShape ==-1)
					myRotate(amt,vec3(0,0,1),zAxis1);
				else

					shapes[pickedShape]->myRotate(amt,vec3(0,0,1),zAxis1);
			break;
			case xGlobalRotate:
				if(pickedShape ==-1)
					globalSystemRot(amt,vec3(1,0,0),xAxis1);
				else
				{
					
					if(direction == -1 && pickedShape+2<shapes.size())
					{
						OpositeDirectionRot(glm::vec3(1,0,0),amt);
					}
					else
						shapes[pickedShape]->globalSystemRot(amt,vec3(1,0,0),xAxis1);
				}
				break;
			case yGlobalRotate:
				if(pickedShape ==-1)
					globalSystemRot(amt,vec3(0,1,0),-1);
				else
				{
					shapes[pickedShape]->globalSystemRot(amt,vec3(0,1,0),-1);
					if(direction == -1)
					{
						OpositeDirectionRot(glm::vec3(0,1,0),amt);
					}
					else
						shapes[pickedShape]->globalSystemRot(amt,vec3(1,0,0),xAxis1);
				}
				break;
			case zGlobalRotate:
				if(pickedShape ==-1)
					globalSystemRot(amt,vec3(0,0,1),zAxis12);
				else
				{
					
					
					if(direction == -1 && pickedShape+2<shapes.size())
					{
						OpositeDirectionRot(glm::vec3(0,0,1),amt);
					}
					else
						shapes[pickedShape]->globalSystemRot(amt,vec3(0,0,1),zAxis12);
				}
			break;
			case xScale:
				if(pickedShape ==-1)
					myScale(vec3(amt,1,1));
				else
					shapes[pickedShape]->myScale(vec3(amt,1,1));
			break;
			case yScale:
				if(pickedShape ==-1)
					myScale(vec3(1,amt,1));
				else
					shapes[pickedShape]->myScale(vec3(1,amt,1));
			break;
			case zScale:
				if(pickedShape ==-1)
					myScale(vec3(1,1,amt));
				else
					shapes[pickedShape]->myScale(vec3(1,1,amt));
			break;
			case xCameraTranslate: //camera plane translate
				if(pickedShape == -1)
					myTranslate(vec3(amt/5.0,0,0),0);
				else
				{
					//newAxis = findAxis(vec3(1,0,0));					
						int i = pickedShape;
					for (; chainParents[i] > 0; i = chainParents[i]);
				
					shapes[i]->translateInSystem(*this,vec3(amt,0,0),0,false);
				}
				break;
			case yCameraTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(0,amt/5.0,0),0);
				else
					{
						//newAxis = findAxis(vec3(0,1,0));
							int i = pickedShape;
						for (; chainParents[i] > 0; i = chainParents[i]);
						
						shapes[i]->translateInSystem(*this,vec3(0,amt,0),0,false);
					}
			break;
			case zCameraTranslate:
				if(pickedShape ==-1)
					myTranslate(vec3(0,0,amt/5.0),0);
				else
					{
					//	newAxis = findAxis(vec3(0,0,1));
							int i = pickedShape;
						for (; chainParents[i] > 0; i = chainParents[i]);
			
						shapes[i]->translateInSystem(*this,vec3(0,0,amt),0,false);
					}
			break;
			default:
				break;
			}

		
	}

	void Scene::OpositeDirectionRot(glm::vec3 vec,float angle)
	{
		glm::mat4 localMat = shapes[pickedShape+2]->GetRot();
		glm::mat4 rotMat = localMat*glm::rotate(glm::mat4(1),angle,vec)*glm::transpose(localMat); 
		
		glm::vec3 posStart = GetTipPositionInSystem(pickedShape);
		std::cout<<"start ("<<posStart.x <<", "<<posStart.y<<", "<<posStart.z<<")"<<std::endl;
		
		int i=pickedShape;
		for (; chainParents[i] > 0; i = chainParents[i]);
		glm::mat4 globalMat = shapes[i]->GetRot();
		globalMat = localMat* shapes[i]->GetRot();

		shapes[pickedShape]->zeroRot(true);

		shapes[i]->buildAngMatrices(shapes[pickedShape]->GetRot()*rotMat);
		if(vec.x>0,99)
			shapes[pickedShape+2]->myRotate(-angle,vec,xAxis1);
		else
			shapes[pickedShape+2]->myRotate(-angle,vec,zAxis1);

		glm::vec3 posEnd = GetTipPositionInSystem(pickedShape);
		std::cout<<"end ("<<posEnd.x <<", "<<posEnd.y<<", "<<posEnd.z<<")"<<std::endl;
		shapes[i]->myTranslate((posStart-posEnd),0);
	}
	
	void Scene::resize(int width,int height)
	{
		glViewport(0,0,width,height);
		cameras[0]->setProjection((float)width/(float)height,cameras[cameraIndx]->GetNear(),cameras[cameraIndx]->GetFar());
	}

	float Scene::picking(int x,int y)
	{
		//float depth;
		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Draw(1,0,false); 
						
		GLint viewport[4];  
		unsigned char data[4];
		glGetIntegerv(GL_VIEWPORT, viewport); //reading viewport parameters
		glReadPixels(x,viewport[3] - y,1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glReadPixels(x,viewport[3] - y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
		cout<<"depth "<<depth<<endl;
		int pickedID = data[0] + data[1]*256 + data[2]*256*256 - 1;
		if(data[0] == 0 && data[1] == 0 && data[2] == 0)
		{
			pickedShape = -1;
			//std::cout<<"not picked"<<std::endl;
		}
		else
		{
			pickedShape = pickedID;
			std::cout<<"picked "<<pickedID<<std::endl;
			xold = x;
			yold = y;
		}
		return depth;
	}

	//return coordinates in global system for a tip of arm position is local system 
	glm::vec3 Scene::GetTipPositionInSystem(int indx)
	{
		mat4 Normal1 = mat4(1);
		if(indx>-1)
		{
			int j = indx;
			glm::vec3 vec = glm::vec3(0);
			for (;  chainParents[j] > 0; j = chainParents[j])
			{
				vec = shapes[j]->getVectorInSystem(glm::mat4(1),vec + glm::vec3(0,0,2));
			}
			glm::vec3 origin = vec3(shapes[j]->getTraslate());
			return origin + shapes[j]->getVectorInSystem(glm::mat4(1),vec+glm::vec3(0,0,2))*(float)scaleFactor;
			//return shapes[indx]->getTipPos(Normal1);
		}
		else
		{
			return vec3(0,0,0); 
		}
	}

	glm::vec3 Scene::GetVectorInSystem(int indx,glm::vec3 vec)
	{
		mat4 Normal1 = mat4(1);
		if(indx>-1)
		{
			for (int j = indx;  chainParents[j] > -1; j = chainParents[j])
			{
				Normal1 = shapes[chainParents[j]]->makeTrans() * Normal1;
			}
			return shapes[indx]->getVectorInSystem(Normal1,vec);
		}
		else
		{
			return vec; 
		}
	}
	
	void Scene::mouseProccessing(int button)
	{
		if(pickedShape == -1 || shapes[pickedShape]->Is2D())
		{
			if(button == 1 )
			{				
				GLint viewport[4];
				//float zTmp = 2.0*depth -1.0;
				glGetIntegerv(GL_VIEWPORT, viewport);
				float z=cameras[cameraIndx]->GetFar()+depth*(cameras[cameraIndx]->GetNear()-cameras[cameraIndx]->GetFar());
				float transX = cameras[cameraIndx]->GetWHRelation()*(xrel)/(float) (viewport[2])*cameras[cameraIndx]->GetNear()*2.0f*tan(cameras[cameraIndx]->GetAngle()*M_PI/360.0)*(cameras[cameraIndx]->GetFar()/z);
				float transY =(yrel)/(float) (viewport[3])*cameras[cameraIndx]->GetNear()*2.0f*tan(cameras[cameraIndx]->GetAngle()*M_PI/360.0)*(cameras[cameraIndx]->GetFar()/z);

				shapeTransformation(xCameraTranslate,-transX);
				shapeTransformation(yCameraTranslate,transY);
				WhenTranslate();
			}
			else
			{
				shapeTransformation(zGlobalRotate,xrel*.5f);
				shapeTransformation(xGlobalRotate,yrel*.5f);
				WhenRotate();
			}
		}
	}

	void Scene::ZeroShapesTrans()
	{
		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			shapes[i]->zeroTrans();
			shapes[i]->zeroRot(false);
		//	shapes[i]->zeroRot(true);
		}
	}

	int Scene::get_picked_shape()
	{
		return pickedShape;
	}

	void Scene::set_picked_shape(int newPicked)
	{
		pickedShape = newPicked;
	}

	int Scene::get_num_of_shapes()
	{
		return this->shapes.size();
	}

	void Scene::hide_shape(int indx)
	{
		shapes[indx]->Hide();

	}

	Shape* Scene::get_shape(int indx)
	{
		return shapes[indx];
	}

	std::vector<int> Scene::get_chain_parents()
	{
		return this->chainParents;
	}

	void Scene::set_chain_parents(int child, int parent)
	{
		chainParents[child] = parent;
	}



	void Scene::ReadPixel()
	{
		glReadPixels(1,1,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
	}

	void Scene::updatePosition(float xpos, float ypos)
	{
		xrel = xold - xpos;
		yrel = yold - ypos;
		xold = xpos;
		yold = ypos;
	}

	void Scene::HideShape(int shpIndx)
	{
		if(shapes[shpIndx]->Is2Render())
			shapes[shpIndx]->Hide();
		else 
			shapes[shpIndx]->Unhide();
	}

	Scene::~Scene(void)
	{
	for (Shape* shp : shapes)
		{
			delete shp;
		}
	for (Camera* cam : cameras)
		{
			delete cam;
		}
	for(Shader* sdr: shaders)
		{
			delete sdr;
		}

	delete axisMesh;

}
	 
	void Scene::ScaleAllDirections(int factor)
	{
		shapeTransformation(xScale,factor);
		shapeTransformation(yScale,factor);
		shapeTransformation(zScale,factor);
	}