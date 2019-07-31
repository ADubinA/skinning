#define GLEW_STATIC
#include <GL\glew.h>
#include "scene.h"
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <SDL.h>


using namespace std;
using namespace glm;
static Uint8 *audioPos;
static Uint32 audioLen;

glm::mat4 dualquat2mat4(glm::dualquat dualq)
{
	// dq[0] dq[1] dq[2] dq[3] are our rotation quaternion elements
	// dq[4] dq[5] dq[6] dq[7] are our translation quaternion elements (the dual quaternion's imaginary component)
	float dq[8] = {

		dualq.real.x,
		dualq.real.y,
		dualq.real.z,
		dualq.real.w,

		dualq.dual.x,
		dualq.dual.y,
		dualq.dual.z,
		dualq.dual.w };
	glm::mat4 out = glm::mat4(1);

	out[0][0] = 1.0 - (2.0 * dq[1] * dq[1]) - (2.0 * dq[2] * dq[2]);
	out[0][1] = (2.0 * dq[0] * dq[1]) + (2.0 * dq[3] * dq[2]);
	out[0][2] = (2.0 * dq[0] * dq[2]) - (2.0 * dq[3] * dq[1]);
	out[0][3] = 0;
	out[1][0] = (2.0 * dq[0] * dq[1]) - (2.0 * dq[3] * dq[2]);
	out[1][1] = 1.0 - (2.0 * dq[0] * dq[0]) - (2.0 * dq[2] * dq[2]);
	out[1][2] = (2.0 * dq[1] * dq[2]) + (2.0 * dq[3] * dq[0]);
	out[1][3] = 0;
	out[2][0] = (2.0 * dq[0] * dq[2]) + (2.0 * dq[3] * dq[1]);
	out[2][1] = (2.0 * dq[1] * dq[2]) - (2.0 * dq[3] * dq[0]);
	out[2][2] = 1.0 - (2.0 * dq[0] * dq[0]) - (2.0 * dq[1] * dq[1]);
	out[2][3] = 0;
	out[3][0] = 2.0 * (-dq[7] * dq[0] + dq[4] * dq[3] - dq[5] * dq[2] + dq[6] * dq[1]);
	out[3][1] = 2.0 * (-dq[7] * dq[1] + dq[4] * dq[2] + dq[5] * dq[3] - dq[6] * dq[0]);
	out[3][2] = 2.0 * (-dq[7] * dq[2] - dq[4] * dq[1] + dq[5] * dq[0] + dq[6] * dq[3]);
	out[3][3] = 1;

	return out;
}
bool test_dualquat(glm::mat4 m)
{

	glm::mat3x4 small_tran = glm::mat3x4(glm::transpose(m));
	glm::dualquat quatDul = glm::normalize(glm::dualquat_cast(small_tran));


	glm::vec4 qrot = glm::vec4(quatDul.real.w,
		quatDul.real.x,
		quatDul.real.y,
		quatDul.real.z);

	glm::vec4 qtran = glm::vec4(quatDul.dual.w,
		quatDul.dual.x,
		quatDul.dual.y,
		quatDul.dual.z);



	// convert quat to matrix
	glm::mat4 m2 = dualquat2mat4(quatDul);

	//convert new matrix back to quat
	small_tran = glm::mat3x4(glm::transpose(m2));
	quatDul = glm::normalize(glm::dualquat_cast(small_tran));

	if (glm::distance(qrot , glm::vec4(quatDul.real.w, quatDul.real.x, quatDul.real.y, quatDul.real.z))>0.01)
	{
		std::cout << qrot.x << ", " << qrot.y << ", " << qrot.z << ", " << qrot.w << std::endl;

		std::cout << quatDul.real.w << ", " << quatDul.real.x << ", " << quatDul.real.y << ", " << quatDul.real.z << std::endl;
		//int x;
		//std::cin >> x;
		return false;
	}

	if (glm::distance(qtran, glm::vec4(quatDul.dual.w, quatDul.dual.x, quatDul.dual.y, quatDul.dual.z))>0.01)
	{
		std::cout << qtran.x << ", " << qtran.y << ", " << qtran.z << ", " << qtran.w << std::endl;
		std::cout << quatDul.dual.w << ", " << quatDul.dual.x << ", " << quatDul.dual.y << ", " << quatDul.dual.z << std::endl;
		//int x;
		//std::cin >> x;
		return false;
	}

	return true;
}

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
		
		//cameras.push_back(new Camera(vec3(0,0,1.0f)));
		pickedShape = -1;
		depth = 0;
		cameraIndx = 0;
		xold = 0;
		yold = 0;
		isActive = false;
		menu_mode = false;
		score = 0;
	}

	Scene::Scene(vec3 position,float angle,float hwRelation,float near, float far)
	{
		//verticesSize = sizeof(vertices)/sizeof(vertices[0]);
		//
		//indicesSize = sizeof(indices)/sizeof(indices[0]) ; 
		glLineWidth(6);
		
	//	axisMesh = new Shape(axisVertices,sizeof(axisVertices)/sizeof(axisVertices[0]),axisIndices, sizeof(axisIndices)/sizeof(axisIndices[0]));
		pickedShape = -1;
		depth = 0;
		cameraIndx = 0;
		xold = 0;
		yold = 0;
		isActive = false;
	}

	glm::mat4 Scene::getShapeTotalTrans(int pickedShape)
	{
		glm::mat4 scene_trans = makeTrans();
		glm::mat4 shape_trans = shapes[pickedShape]->makeTrans();
		mat4 total_trans = mat4(1);
		for (int j = pickedShape; chainParents[j] > -1; j = chainParents[j])
		{
			total_trans = shapes[chainParents[j]]->makeTrans() * total_trans;
		}

		return scene_trans*total_trans*shape_trans;
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

	void Scene::AddTexture(const std::string& textureFileName)
	{
		textures.push_back(new Texture(textureFileName));
	}
	void Scene::collisionDetection()
	{
		

		bool picked;
		for (int i=0; i<shapes.size(); i++)
		{
			Shape* shape1 = shapes[i];
			if (shape1->mode >= TRIANGLES && i!=spacse_indx&& shape1->Is2Render())
			{
				for (int j=0; j<shapes.size(); j++)
				{
					Shape* shape2 = shapes[j];
					// TODO check that equality is very true
					if (shape2->mode == TRIANGLES && i!=j && j!= spacse_indx && shape2->Is2Render())
					{

						picked = shape1->checkCollision(shape2,
														getShapeTotalTrans(i),
														getShapeTotalTrans(j));
						if (picked)
						{
							//shapes[picked]->Unhide();
							if (i == planets[score] ) {
								shape1->Hide();
								score++;
								playTune("Sounds/Jump.wav");
							}
							else if (j == planets[score]) {
								shape2->Hide();
								score++;
								playTune("Sounds/Jump.wav");
							}
							else
								this->Deactivate();
							//std::cout << j << "," << i << std::endl;
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

	void Scene::Draw(int shaderIndx,bool debugMode)
	{
		glm::mat4 Normal = makeTrans();
		glm::mat4 MV =   Normal;
		glm::mat4 P = cameras[cameraIndx]->GetViewProjection() ;
		glm::vec4 Qrot[NUM_OF_SHAPES];
		glm::vec4 Qtrans[NUM_OF_SHAPES];
//		int p = pickedShape;
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

				Update(MV1,P,Normal1 ,i,shaders[shapes[i]->shaderID], shapes[i]->shaderID);
				shapes[i]->Draw(shaders, textures, false);
				//shapes[i]->Draw(*shaders[shapes[i]->shader_indx]);
				bool t;
				if(shapes[i]->mode ==5)
					 t = test_dualquat(MV1);

				glm::mat3x4 small_tran = glm::mat3x4(transpose(MV1));
				glm::dualquat quatDul = glm::dualquat_cast(small_tran);

				if (t) {
					Qrot[i] = glm::vec4(quatDul.real.w,
						quatDul.real.x,
						quatDul.real.y,
						quatDul.real.z);

					Qtrans[i] = glm::vec4(quatDul.dual.w,
						quatDul.dual.x,
						quatDul.dual.y,
						quatDul.dual.z);
				}
				else {
					Qrot[i] = Qrot[i - 1];
					Qtrans[i] = Qtrans[i - 1];
				}
				

			}
		}
		shaders[SKINNING_SHADER]->Bind();
		shaders[SKINNING_SHADER]->SetUniform4v("Qrot", Qrot);
		shaders[SKINNING_SHADER]->SetUniform4v("Qtrans", Qtrans);
		
//		pickedShape = p;
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

	Camera* Scene::get_camera(int indx)
	{
		return this->cameras[indx];
	}

	int Scene::get_camera_indx()
	{
		return this->cameraIndx;
	}

	void Scene::set_camera_indx(int i)
	{
		this->cameraIndx = i;
	}

	Texture* Scene::getTexture(int index)
	{
		return this->textures[index];
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
		Draw(1,false); 
						
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
	void my_audio_callback(void *userdata, Uint8 *stream, int len);


	void my_audio_callback(void *userdata, Uint8 *stream, int len)
	{
		if (audioLen == 0)
			return;

		len = (len > audioLen ? audioLen : len);
		//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
		SDL_MixAudio(stream, audioPos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

		audioPos += len;
		audioLen -= len;
	}

	void Scene::playTune(char* str)
	{
		if (SDL_Init(SDL_INIT_AUDIO) < 0)
			return;

		//local variables
		static Uint32 wav_length; // length of our sample
		static Uint8 *wav_buffer; // buffer containing our audio file
		static SDL_AudioSpec wav_spec; // the specs of our piece of music

		if (SDL_LoadWAV(str, &wav_spec, &wav_buffer, &wav_length) == NULL) {
			return;
		}
		// set the callback function
		wav_spec.callback = my_audio_callback;
		wav_spec.userdata = NULL;
		// set our global static variables
		audioPos = wav_buffer; // copy sound buffer
		audioLen = wav_length; // copy file length

								/* Open the audio device */
		if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
			fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
			exit(-1);
		}

		/* Start playing */
		SDL_PauseAudio(0);

		// wait until we're don't playing
		while (audioLen > 0) {
			SDL_Delay(100);
		}

		// shut everything down
		SDL_CloseAudio();
		SDL_FreeWAV(wav_buffer);
	}