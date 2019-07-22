#pragma once
#include "shader.h"
#include "shape.h"
#include "camera.h"
#include "VertexArray.hpp"
#include <vector>
#define NUM_OF_SHAPES 30

enum camera_mode { ThirdPersonCamera, FirstPersonCamera };
class Scene : public MovableGLM
{

public:
	bool menu_mode;
	int spacse_indx;

	enum axis{xAxis,yAxis,zAxis};
	enum transformations{xLocalTranslate,yLocalTranslate,zLocalTranslate,xGlobalTranslate,yGlobalTranslate,zGlobalTranslate,
		xLocalRotate,yLocalRotate,zLocalRotate,xGlobalRotate,yGlobalRotate,zGlobalRotate,xScale,yScale,zScale,xCameraTranslate,yCameraTranslate,zCameraTranslate};
	enum modes{POINTS,LINES,LINE_LOOP,LINE_STRIP,TRIANGLES,TRIANGLE_STRIP,TRIANGLE_FAN,QUADS};
	enum Shapes
	{
		Axis,
		Cube,
		Octahedron,
		Tethrahedron,
		BezierLine,
		BezierSurface,
		LineCopy,
		MeshCopy,
	};
	
	Scene();
	Scene(glm::vec3 position,float angle,float hwRelation,float near, float far);
	glm::mat4 getShapeTotalTrans(int pickedShape);
	void AddTexture(const std::string& textureFileName);
	void addShapeFromFile(const std::string& fileName,int parent,unsigned int mode);
	virtual void addShape(int type,int parent,unsigned int mode);
	virtual	void addShape(Bezier1D * curve, int parent, unsigned int mode);

	void addShapeCopy(int indx,int parent,unsigned int mode);
	void collisionDetection();
	void addShader(const std::string& fileName);
	void ZeroShapesTrans();
	int get_picked_shape();
	void set_picked_shape(int newPicked);
	int get_num_of_shapes();
	void hide_shape(int indx);
	Shape* get_shape(int indx);
	std::vector<int> get_chain_parents();
	void set_chain_parents(int child, int parent);

	//virtual void Update( glm::mat4 MVP ,glm::mat4 *jointTransforms,const int length,const int  shaderIndx);//

	virtual void Update(const glm::mat4 &MV, const glm::mat4 &P,const glm::mat4 &Normal, int indx, Shader *s,int s_index) = 0;
	virtual void WhenTranslate(){};
	virtual void WhenRotate(){};
	virtual void Motion(){};
	virtual void Draw(int shaderIndx,bool debugMode);

	glm::mat4 GetViewProjection(int indx) const; 
	glm::mat4 GetShapeTransformation() const;
	
	void shapeTransformation(int type,float amt);
	void shapeRotation(glm::vec3 v, float ang,int indx);
	//void inline setPicked(int pickID){pickedShape = pickID;}
	float picking(int x,int y);
	void resize(int width,int hight);

	inline void setParent(int indx,int newValue) {chainParents[indx]=newValue;}
	inline float GetNear(int cameraIndx){return cameras[cameraIndx]->GetNear();}
	inline float GetFar(int cameraIndx){return cameras[cameraIndx]->GetFar();}
	inline float GetWHRelation(int cameraIndx){return cameras[cameraIndx]->GetWHRelation();}

	void ReadPixel();
	
	inline float GetAngle(int cameraIndx) {return cameras[cameraIndx]->GetAngle();}
	inline void Activate() {isActive = true;}
	inline void Deactivate() {isActive = false;}
	void HideShape(int shpIndx);
	//inline void SetMousePosition(double xpos, double ypos){xold =xpos; yold=ypos;}
	void updatePosition(float xpos, float ypos);
	void mouseProccessing(int button);
	bool inline IsActive() const { return isActive;} 
	virtual ~Scene(void);
	inline void SetDirection() {direction = -direction;}
	inline int GetDirection() {return direction;}
	glm::vec3 GetTipPositionInSystem(int indx);
	glm::vec3 GetVectorInSystem(int indx,glm::vec3 vec);
	void Scene::OpositeDirectionRot(glm::vec3 vec,float angle);
	Camera* get_camera(int indx);
	int get_camera_indx();
	void set_camera_indx(int i);
	Texture* getTexture(int index);
private:	

	
	
	Shape *axisMesh;
	int verticesSize;
	int indicesSize;

	float depth;
	int xold, yold,xrel, yrel;

protected:
	int cameraIndx;
	std::vector<Camera*> cameras; //light will have the properties of camera
	std::vector<Shape*> shapes;
	std::vector<Shader*> shaders;
	std::vector<Texture*> textures;
	std::vector<int> chainParents;
	int pickedShape;
	int direction;
	static const int scaleFactor =2;
	bool isActive;
	void ScaleAllDirections(int factor);
};

