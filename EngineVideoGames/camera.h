#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera 
{
public:
	/*void myRotate(float ang, glm::vec3 &vec, int indx);
	void myTranslate(glm::vec3 &vec, int indx);*/
	Camera(const glm::vec3& pos);
	void setProjection(float aspect, float zNear, float zFar);

	glm::mat4 GetViewProjection();



	void MoveForward(float amt);
	void TranslateCamera(glm::vec3 position);
	void RotateCamera(glm::mat4 rotation_matrix,glm::vec3 center);
	void MoveRight(float amt);

	void Pitch(float angle);

	void RotateY(float angle);
	void RotateZ(float angle);
	float GetAngle();
	float GetNear();
	float GetFar();
	float GetWHRelation();
protected:
private:
	glm::vec3 const default_forward = glm::vec3(0, 0, -1);
	glm::vec3 const default_up = glm::vec3(0, 1, 0);
	glm::vec3 default_pos;

	glm::mat4 projection;
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
	float fov;
	float far,near,relation;
};

#endif
