#include "bezier1D.h"
#include "math.h"
#include <iostream>


Bezier1D::Bezier1D(void)
{
	this->num_of_segments = 3;
	for (int i = 0; i < this->num_of_segments; i++)
	{
		int sign = (i % 2 == 0) ? 1 : -1;
		segments.push_back(glm::mat4(
			glm::vec4(0.0f, i, 0.0f, 0.0f),
			glm::vec4(sign * 1.0f, i, 0.0f, 0.0f),
			glm::vec4(sign * 1.0f, i + 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, i + 1.0f, 0.0f, 0.0f)
		));
	}
}

Bezier1D::Bezier1D(int mode)
{
	this->num_of_segments = 1;
	if (mode == BODY)
	{
		segments.push_back(glm::mat4(
			glm::vec4(0.0f, 0.25f, 0.0f, 0.0f),
			glm::vec4(0.25f,0.25f, 0.0f, 0.0f),
			glm::vec4(0.75f,0.25f, 0.0f, 0.0f),
			glm::vec4(1.0f, 0.25f, 0.0f, 0.0f)
		));
	}
	else if (mode == EDGE)
	{
		segments.push_back(glm::mat4(
			glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
			glm::vec4(0.25f, 0.25f, 0.0f, 0.0f),
			glm::vec4(0.75f, 0.25f, 0.0f, 0.0f),
			glm::vec4(1.0f, 0.25f, 0.0f, 0.0f)
		));
	}
}

IndexedModel Bezier1D::GetLine(int resT)
{
	IndexedModel line;
	LineVertex vertex = LineVertex(glm::vec3(0), glm::vec3(0));
	float time = 0.0f;
	int counter = 0;
	for (int segment_index = 0; segment_index < this->num_of_segments; segment_index++)
	{
		for (int t = 0; t < resT; t++)
		{

			// calcualte line
			time = (float)t / resT;
			vertex = this->GetVertex(segment_index, time);

			// insert to indexed model line
			line.positions.push_back(*vertex.GetPos());
			line.colors.push_back(*vertex.GetColor());
			line.indices.push_back(counter);
			counter++;
		}
		time = 0.0f;
	}
	return line;

}

LineVertex Bezier1D::GetVertex(int segment, float t)
{
	glm::mat4 segment_coord = this->segments[segment];
	glm::vec3 position = glm::vec3(segment_coord[0]) * float(pow((1 - t), 3)) +
		glm::vec3(segment_coord[1]) * float(3 * pow((1 - t), 2) * t) +
		glm::vec3(segment_coord[2]) * float(3 * pow((t), 2) * (1 - t)) +
		glm::vec3(segment_coord[3]) * float(pow((t), 3));

	return LineVertex(position, glm::vec3(0, 0, 1));
}

LineVertex Bezier1D::GetControlPoint(int segment, int indx)
{
	LineVertex control_point = LineVertex(glm::vec3(this->segments[segment][indx]), glm::vec3(0, 0, 1));
	return control_point;
}


glm::vec3 Bezier1D::GetVelosity(int segment, float t)
{
	glm::mat4 segment_coord = this->segments[segment];
	glm::vec3 position = (glm::vec3(segment_coord[1]) - glm::vec3(segment_coord[0]))* float(3 * pow((1 - t), 2)) +
		(glm::vec3(segment_coord[2]) - glm::vec3(segment_coord[1]))* float(6 * (1 - t) * t) +
		(glm::vec3(segment_coord[3]) - glm::vec3(segment_coord[2]))* float(3 * pow((t), 2) * t);

	return position;
}

void Bezier1D::MoveControlPoint(int segment, int indx, bool preserveC1, glm::vec4 newPosition)
{
	newPosition.w = 0;
	glm::vec4 distance_vector = newPosition - this->segments[segment][indx];
	glm::vec4 old_pos = this->segments[segment][indx];
	float d_pre;
	float e_pre;
	float d_after;
	float e_after;
	glm::vec4 direction;

	this->segments[segment][indx] = newPosition;

	if (indx == 0)
	{
		if (segment != 0)
		{
			this->segments[segment - 1][3] = newPosition;
		}
	}
	else if (indx == 3)
	{
		if (segment != this->num_of_segments - 1)
		{
			this->segments[segment + 1][0] = newPosition;

		}
	}

	if (preserveC1 && !(segment == 0 && indx<2) && !(segment == num_of_segments - 1 && indx>1))
	{
		glm::vec4 m;
		switch (indx)
		{
		case 0:
			this->segments[segment - 1][2] += distance_vector;
			this->segments[segment][1] += distance_vector;
			break;

		case 1:
			d_after = glm::distance(newPosition, this->segments[segment - 1][3]);
			direction = glm::normalize(newPosition - this->segments[segment - 1][3]);
			this->segments[segment - 1][2] = this->segments[segment - 1][3] - direction* d_after;
			break;

		case 2:
			d_after = glm::distance(newPosition, this->segments[segment][3]);
			direction = glm::normalize(newPosition - this->segments[segment][3]);
			this->segments[segment + 1][1] = this->segments[segment][3] - direction* d_after;


			break;

		case 3:
			this->segments[segment + 1][1] += distance_vector;
			this->segments[segment][2] += distance_vector;
			break;
		}
	}



	//if (segment == 0 && indx<=1) {
	//	segment_coord[indx] = glm::translate(glm::mat4(1),newPosition) * segment_coord[indx];
	//}





}


Bezier1D::~Bezier1D(void)
{
}
