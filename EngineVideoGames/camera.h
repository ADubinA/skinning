#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
	Camera(const glm::vec3& pos);
	void setProjection(float aspect, float zNear, float zFar);

	glm::mat4 GetViewProjection();



	void MoveForward(float amt);
	void TranslateCamera(glm::vec3 position);
	void RotateCamera(glm::mat4 rotation_matrix);
	void MoveRight(float amt);

	void Pitch(float angle);

	void RotateY(float angle);
	float GetAngle();
	float GetNear();
	float GetFar();
	float GetWHRelation();
protected:
private:
	glm::mat4 projection;
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
	float fov;
	float far,near,relation;
};

#endif
