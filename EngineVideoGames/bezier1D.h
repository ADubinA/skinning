#pragma once
#include "Mesh.h"
#include "glm/gtc/matrix_transform.hpp"
#define BODY 0
#define HEAD 1
#define TAIL 2

class Bezier1D
{
	//each line in the matrix represents x,y,z,w of a control point

public:
	std::vector<glm::mat4> segments;
	int num_of_segments;
	Bezier1D(void);
	Bezier1D(int mode);
	IndexedModel GetLine(int resT);						//generates model for rendering using MeshConstructor::initLin
	LineVertex GetVertex(int segment, float t);			//returns point on curve in the requested segment for value of t
	LineVertex GetControlPoint(int segment, int indx);	//returns a control point in the requested segment. indx will be 0,1,2,3, for p0,p1,p2,p3
	glm::vec3 GetVelosity(int segment, float t);			//returns the derivative of the curve in the requested segment for value of t

	void MoveControlPoint(int segment, int indx, bool preserveC1, glm::vec4 newPosition); //change the positon of one control point. when preserveC1 is true it may affect other  control points 


	~Bezier1D(void);
};