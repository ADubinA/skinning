#include "camera.h"

//void Camera::myRotate(float ang, glm::vec3 & vec, int indx)
//{
//	MovableGLM::myRotate(ang, vec, indx);
//	forward = glm::vec3(MovableGLM::GetRot()*glm::vec4(this->default_forward,0));
//	up = glm::vec3(MovableGLM::GetRot()*glm::vec4(this->default_up, 0));
//
//}
//
//void Camera::myTranslate(glm::vec3 & vec, int indx)
//{
//	MovableGLM::myTranslate(vec, indx);
//	pos =  glm::vec3(makeTrans()*glm::vec4(this->default_pos, 1);
//
//}

Camera::Camera(const glm::vec3& pos)

{

	this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->default_pos = pos;
	this->pos = pos;
	this->fov = 60.0f;
	this->near = 0.6f;
	this->far = 500.0f;
	this->relation = (float)1200 / (float)800;
	this->projection = glm::perspective(fov, relation, near, far);
	this->projection = this->projection * glm::lookAt(pos, pos + forward, up);
}

void Camera::setProjection(float aspect, float zNear, float zFar)
{
	this->projection = glm::perspective(fov, aspect, zNear, zFar)* glm::lookAt(pos, pos + forward, up);
	this->near = zNear;
	this->far = zFar;
	this->relation = aspect;
}

glm::mat4 Camera::GetViewProjection()
{
	this->projection = glm::perspective(fov, this->relation, this->near, this->far)* 
					   glm::lookAt(pos, pos + forward, up);
	return projection;
}

void Camera::MoveForward(float amt)
{
	pos += forward * amt;
}

void Camera::TranslateCamera(glm::vec3 position)
{
	this->pos += position;
}

void Camera::RotateCamera(glm::mat4 rotation_matrix,glm::vec3 center)
{
	this->up = glm::normalize(glm::vec3(rotation_matrix * glm::vec4(up,0)));
	this->forward = glm::vec3(rotation_matrix * glm::vec4(forward, 0));
	//this->pos = glm::vec3(rotation_matrix *glm::translate(center)*glm::vec4(this->default_pos, 1));
}

void Camera::MoveRight(float amt)
{
	pos += glm::cross(up, forward) * amt;
}



void Camera::Pitch(float angle)
{
	glm::vec3 right = glm::normalize(glm::cross(up, forward));

	forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));
	up = glm::normalize(glm::cross(forward, right));
}

void Camera::RotateY(float angle)
{
	//static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

	glm::mat4 rotation = glm::rotate(angle, up);

	forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
	//up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
}

void Camera::RotateZ(float angle)
{
	//static const glm::vec3 FORW(0.0f, 0.0f, 1.0f);

	glm::mat4 rotation = glm::rotate(angle, forward);

	//forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
	up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
}
 float Camera::GetAngle()
{
	return fov;
}

 float Camera::GetNear()
{
	return near;
}
 float Camera::GetFar()
{
	return far;
}
 float Camera::GetWHRelation()
{
	return relation;
}
 void Camera::set_camera_movable_params(glm::vec3 pose, glm::vec3 forward, glm::vec3 up)
 {
	 this->pos = pose;
	 this->forward = forward;
	 this->up = up;
 }