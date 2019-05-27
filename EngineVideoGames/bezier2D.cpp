#include "bezier2D.h"
#include "glm/gtx/transform.hpp"
# define M_PI           3.14159265f /* pi */




Bezier2D::Bezier2D(void)
{
	this->b = *new Bezier1D();
}

Bezier2D::Bezier2D(Bezier1D* b, glm::vec3 axis, int circularSubdivision)
{
	this->circularSubdivision = circularSubdivision;
	this->axis = axis;
	this->b = *b;
	//this->b = new Bezier1D();
}

IndexedModel Bezier2D::GetSurface(const int resT, const int resS)
{
	IndexedModel surface;

	Vertex vertex1 = Vertex(glm::vec3(0), glm::vec2(0), glm::vec3(0), glm::vec3(0));
	Vertex vertex2 = Vertex(glm::vec3(0), glm::vec2(0), glm::vec3(0), glm::vec3(0));
	Vertex vertex3 = Vertex(glm::vec3(0), glm::vec2(0), glm::vec3(0), glm::vec3(0));
	Vertex vertex4 = Vertex(glm::vec3(0), glm::vec2(0), glm::vec3(0), glm::vec3(0));
	float timet = 0.0f;
	float times = 0.0f;
	int*** array_num = new int**[b.num_of_segments];
	for (int i = 0; i < b.num_of_segments; ++i) {
		array_num[i] = new int*[resT];
		for (int j = 0; j < resT; j++) {
			array_num[i][j] = new int[resS];

		}
	}

	int counter = 0;
	for (int segment_indexT = 0; segment_indexT < this->b.num_of_segments; segment_indexT++) {
		for (int t = 0; t < resT - 1; t = t + 2) { //resT-1?
			for (int s = 0; s < resS - 1; s = s + 2) { //resS-1?
				array_num[segment_indexT][t][s] = counter;
				counter++;
				array_num[segment_indexT][t + 1][s] = counter;
				counter++;
				array_num[segment_indexT][t + 1][s + 1] = counter;
				counter++;
				array_num[segment_indexT][t][s + 1] = counter;
				counter++;

			}
		}
	}


	for (int segment_indexT = 0; segment_indexT < this->b.num_of_segments; segment_indexT++)
	{

		for (int t = 0; t < resT - 1; t = t + 1) { //resT-1?
			for (int s = 0; s < resS - 1; s = s + 2) { //resS-1?

													   // calcualte line
				timet = (float)t / resT;
				times = (float)s / resS;
				vertex1 = this->GetVertex(segment_indexT, 0, timet, times);
				vertex2 = this->GetVertex(segment_indexT, 0, timet + (1.0f / resT), times);
				vertex3 = this->GetVertex(segment_indexT, 0, timet + (1.0f / resT), times + (1.0f / resS));
				vertex4 = this->GetVertex(segment_indexT, 0, timet, times + (1.0f / resS));

				// insert to indexed model line
				surface.positions.push_back(*vertex1.GetPos());
				surface.positions.push_back(*vertex2.GetPos());
				surface.positions.push_back(*vertex3.GetPos());
				surface.positions.push_back(*vertex4.GetPos());
				surface.colors.push_back(*vertex1.GetColor());
				surface.colors.push_back(*vertex2.GetColor());
				surface.colors.push_back(*vertex3.GetColor());
				surface.colors.push_back(*vertex4.GetColor());
				surface.normals.push_back(*vertex1.GetNormal());
				surface.normals.push_back(*vertex2.GetNormal());
				surface.normals.push_back(*vertex3.GetNormal());
				surface.normals.push_back(*vertex4.GetNormal());
				surface.texCoords.push_back(glm::vec2(1));
				surface.texCoords.push_back(glm::vec2(1));
				surface.texCoords.push_back(glm::vec2(1));
				surface.texCoords.push_back(glm::vec2(1));
				surface.indices.push_back(2 * array_num[segment_indexT][t][s]);
				surface.indices.push_back(2 * array_num[segment_indexT][t + 1][s]);
				surface.indices.push_back(2 * array_num[segment_indexT][t + 1][s + 1]);
				surface.indices.push_back(2 * array_num[segment_indexT][t][s + 1]);
				surface.weights.push_back(calcWeight(segment_indexT, 0, timet, times));
				surface.weights.push_back(calcWeight(segment_indexT, 0, (float)(t + 1) / resT, times));
				surface.weights.push_back(calcWeight(segment_indexT, 0, (float)(t + 1) / resT, times));
				surface.weights.push_back(calcWeight(segment_indexT, 0, timet,                 times));

			}
		}


		timet = 0.0f;
		times = 0.0f;

	}
	for (int i = 0; i < b.num_of_segments; ++i) {
		for (int j = 0; j < resT; j++) {
			delete (array_num[i][j]);

		}
		delete (array_num[i]);

	}
	return surface;
}

Vertex Bezier2D::GetVertex(int segmentT, int segmentS, float t, float s)
{
	glm::mat4 segment_coordT = this->b.segments[segmentT];

	glm::mat4 rotateMat = glm::rotate(360.0f* s, this->axis);


	glm::vec3 pos = glm::vec3(rotateMat * glm::vec4(*b.GetVertex(segmentT, t).GetPos(), 1));
	glm::vec2 texCoord = glm::vec2(0);
	glm::vec3 normal = GetNormal(segmentT, segmentS, t, s);
	glm::vec3 color = *b.GetVertex(segmentT, t).GetColor();
	glm::vec3 weight = glm::vec3(0);
	Vertex vertex = Vertex(pos, texCoord, normal, color);

	return vertex;
}

glm::vec3 Bezier2D::GetNormal(int segmentT, int segmentS, float t, float s)
{
	glm::mat4 segment_coordT = this->b.segments[segmentT];
	glm::vec3 rotate_axis = glm::vec3(this->b.segments[b.num_of_segments - 1][3] - this->b.segments[0][0]);
	glm::mat4 rotateMat = glm::rotate(360.0f* s, rotate_axis);

	glm::vec3 b_vec = b.GetVelosity(segmentT, t);
	glm::vec3 c_vec = glm::vec3(rotateMat*glm::vec4(glm::vec3(0, 0, 1), 0));
	float sign = (segmentT % 2 == 0) ? 1.0f : -1.0f;
	glm::vec3 normal = sign * glm::normalize(glm::cross(b_vec, c_vec));
	normal.y = sign * normal.y;

	return -normal;
}

glm::vec3 Bezier2D::calcWeight(int segmentT, int segmentS, float t, float s)
{
	float f1 = 0, f3 = 0;
	if (t>0.5)
		f3 = (1 - 4.0f*(1 - t)*t)*(1 - t) / 2.0f + (1 - 4.0f*(1 - t)*t)*t / 2.0f;
	else
		f1 = (1 - 4.0f*(1 - t)*t)*(1 - t) / 2.0f + (1 - 4.0f*(1 - t)*t)*t / 2.0f;
	float f2 = (2.0f*(1 - t)*(t + 0.0) + 0.5f);
	return glm::vec3(f1, f2, f3);
}


Bezier2D::~Bezier2D(void)
{
}

