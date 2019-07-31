#include "snak.h"




Snak::Snak(int num_of_joints, Scene* scn)
{
	this->num_of_links = num_of_joints;
	this->head_indx = 0;
	this->tail_indx = 0;
	this->is_dead = 1;
	this->velocity = glm::vec3 (-1,0,0);
	this->scn = scn;
	this->speed = 0.1;

	this->interpolation_constant = 15.0f;

}

void Snak::createSnake()
{
	
	Bezier1D * body = new Bezier1D(BODY);
	Bezier1D * head = new Bezier1D(HEAD);
	Bezier1D * tail = new Bezier1D(TAIL);
	scn->addShape(body, -1, 5);
	int body_index = scn->get_num_of_shapes() - 1;
	scn->hide_shape(scn->get_num_of_shapes() - 1);
	scn->addShape(head, -1, 5);
	int edge_index = scn->get_num_of_shapes() - 1;
	this->head_indx = edge_index;
	scn->get_shape(this->head_indx)->SetTexture(SNAKE_TEX_INDEX);
	scn->get_shape(body_index)->	 SetTexture(SNAKE_TEX_INDEX);

	this->index_chain.push_back(head_indx);


	BoundingBox* bodybox = scn->get_shape(body_index)->mesh->bvh.box;
	BoundingBox* edgebox = scn->get_shape(edge_index)->mesh->bvh.box; 
	scn->set_picked_shape(edge_index);
	scn->shapeTransformation(scn->xGlobalTranslate, edgebox->static_center.x - bodybox->size.x);
	scn->shapeTransformation(scn->yGlobalTranslate, edgebox->static_center.y);
	scn->shapeTransformation(scn->zGlobalTranslate, edgebox->static_center.z);
	for (int i = 0; i < this->num_of_links; i++) {
		//addShapeCopy(body_index, edge_index + i - 1, QUADS);
		//scn->addShape(body_index, -1, 5);
		scn->addShape(body, -1, 5);
		scn->get_shape(scn->get_num_of_shapes() - 1)->SetTexture(SNAKE_TEX_INDEX);
		this->index_chain.push_back(scn->get_num_of_shapes() - 1);

		scn->set_picked_shape(scn->get_num_of_shapes() - 1);
		scn->shapeTransformation(scn->xGlobalTranslate, bodybox->static_center.x + bodybox->size.x);
		scn->shapeTransformation(scn->yGlobalTranslate, bodybox->static_center.y);
		scn->shapeTransformation(scn->zGlobalTranslate, bodybox->static_center.z);
		scn->set_chain_parents(scn->get_picked_shape(),  scn->get_picked_shape() - 1);
	}


	scn->addShape(tail, -1, 5);
	this->index_chain.push_back(scn->get_num_of_shapes() - 1);


	scn->set_picked_shape(scn->get_num_of_shapes() - 1);
	this->tail_indx = scn->get_num_of_shapes() - 1;
	scn->get_shape(this->tail_indx)->SetTexture(SNAKE_TEX_INDEX);
	//shapeTransformation(zLocalRotate, 180);
	scn->shapeTransformation(scn->xGlobalTranslate, edgebox->static_center.x + bodybox->size.x);
	//shapeTransformation(xLocalTranslate,  -2*bodybox->size.x);
	scn->shapeTransformation(scn->yGlobalTranslate, edgebox->static_center.y);
	scn->shapeTransformation(scn->zGlobalTranslate, edgebox->static_center.z);
	scn->set_chain_parents(scn->get_picked_shape(), scn->get_picked_shape() - 1);

	this->is_dead = 0;
}

void Snak::move(Direction direction)
{
	glm::mat4 head_rotation = scn->get_shape(this->head_indx)->GetRot();
	glm::vec3 z_after_rotation = glm::vec3(0, 0, 1);//glm::vec3(head_rotation * glm::vec4(0, 0, 1, 0));
	glm::vec3 y_after_rotation = glm::vec3(0, 1, 0);//glm::vec3(head_rotation * glm::vec4(0, 1, 0, 0));

	glm::mat4 changed_rotation = glm::mat4(1);
	//Camera *cam3 = scn->get_camera(ThirdPersonCamera);
	//Camera *cam1 = scn->get_camera(FirstPersonCamera);
	switch (direction)
	{
	case Down:
		this->velocity =glm::vec3( glm::rotate(ROTATION_SPEED, z_after_rotation)*head_rotation*glm::vec4(-1,0,0,0));
		scn->get_shape(this->head_indx)->myRotate(ROTATION_SPEED, z_after_rotation, -1);
		//TODO remove
		scn->get_shape(this->head_indx+1)->myRotate(-1*ROTATION_SPEED, z_after_rotation, -1);
		//cam3->RotateZ(-ROTATION_SPEED);
		//cam1->Pitch(ROTATION_SPEED);
		

		break;

	case Up:
		this->velocity = glm::vec3(glm::rotate(-ROTATION_SPEED, z_after_rotation)*head_rotation*glm::vec4(-1, 0, 0, 0));
		scn->get_shape(this->head_indx)->myRotate(-ROTATION_SPEED, z_after_rotation, -1);

		//TODO remove
		scn->get_shape(this->head_indx + 1)->myRotate(1 * ROTATION_SPEED, z_after_rotation, -1);
		//cam3->RotateZ(ROTATION_SPEED);
		//cam1->Pitch(-ROTATION_SPEED);

		break;

	case Left:
		this->velocity = glm::vec3(glm::rotate(ROTATION_SPEED, y_after_rotation)*head_rotation*glm::vec4(-1, 0, 0, 0));
		scn->get_shape(this->head_indx)->myRotate(ROTATION_SPEED, y_after_rotation, -1);
		//TODO remove
		scn->get_shape(this->head_indx + 1)->myRotate(-1 * ROTATION_SPEED, y_after_rotation, -1);
		//cam3->RotateY(ROTATION_SPEED);
		//cam3->MoveRight(-1);
		//cam1->RotateY(ROTATION_SPEED);
		
		break;

	case Right:
		this->velocity = glm::vec3(glm::rotate(-ROTATION_SPEED, y_after_rotation)*head_rotation*glm::vec4(-1, 0, 0, 0));
		scn->get_shape(this->head_indx)->myRotate(-ROTATION_SPEED, y_after_rotation, -1);
		//TODO remove
		scn->get_shape(this->head_indx + 1)->myRotate(1 * ROTATION_SPEED, y_after_rotation, -1);
		//cam3->RotateY(-ROTATION_SPEED);
		//cam3->MoveRight(1);
		//cam1->RotateY(-ROTATION_SPEED);

		break;

	case Forward:

		break;
	default:
		break;
	}

	// align the snake to almost straight 
	for (int i = 0; i <= num_of_links; i++)
	{
		align_segments(index_chain[i], index_chain[i+1]);
	}

	int tmp_picked_shape = scn->get_picked_shape();
	scn->set_picked_shape(this->head_indx);

	scn->shapeTransformation(scn->xGlobalTranslate, speed*velocity.x);
	scn->shapeTransformation(scn->yGlobalTranslate, speed*velocity.y);
	scn->shapeTransformation(scn->zGlobalTranslate, speed*velocity.z);
	//cam3->MoveRight(speed);
	//cam1->MoveForward(speed);


	scn->set_picked_shape(tmp_picked_shape);
}

glm::vec3 Snak::get_head_pos()
{
	BoundingBox *head_box = scn->get_shape(this->head_indx)->mesh->bvh.box;
	return head_box->center;
}

glm::vec3 Snak::get_segment_tip(int indx)
{
	BoundingBox *body_box = scn->get_shape(indx)->mesh->bvh.box;
	return body_box->center + body_box->size;
}
glm::mat4 Snak::get_segment_rotation(int indx)
{
	glm::mat4 rot = glm::mat4(1);
	for (int i = indx;; i--)
	{
		if (i == head_indx)
		{
			break;
		}
		rot *= scn->get_shape(i)->GetRot();
	
	}
	return rot;
}
void Snak::align_segments(int stationary, int moving)
{
	glm::vec3 stationary_vector = glm::normalize(glm::vec3(this->get_segment_rotation(stationary)*glm::vec4(1, 0, 0, 0)));
	glm::vec3 moving_vector = glm::normalize(glm::vec3(this->get_segment_rotation(moving)*glm::vec4(1, 0, 0, 0)));
		glm::mat4 R = get_rotation_matrix(moving_vector,stationary_vector,interpolation_constant);
		scn->get_shape(moving)->myRotate(R);
		if (moving + 1 <= this->tail_indx)
		{
			scn->get_shape(moving + 1)->myRotate(glm::transpose(R));
		}
	


	/*glm::vec3 rotation_axis = glm::cross(stationary_vector, moving_vector);
	if (abs(glm::dot(rotation_axis, rotation_axis)) > 0.001)
	{
		rotation_axis = glm::normalize(rotation_axis);
		float rotation_angle = (180.0f/PI)*glm::acos(glm::dot(stationary_vector, moving_vector));

		rotation_angle = -rotation_angle * interpolation_constant;

		scn->get_shape(moving)->myRotate(rotation_angle, rotation_axis, -1);


	}*/
}
glm::mat4 get_rotation_matrix(glm::vec3 stationary, glm::vec3 moving,float interpolation_constant)
{
	//glm::mat4 G = glm::mat4(glm::vec4(glm::dot(stationary, moving), -glm::normalize(glm::cross(stationary, moving)), 0, 0),
	//						glm::vec4(glm::normalize(glm::cross(stationary, moving)), glm::dot(stationary, moving), 0, 0),
	//						glm::vec4(0, 0, 1, 0),
	//						glm::vec4(0, 0, 0, 1));
	//glm::mat4 F = glm::mat4(glm::vec4(stationary, 0),
	//						glm::vec4(glm::normalize(moving - glm::dot(stationary, moving)*stationary), 0),
	//						glm::vec4(glm::cross(moving, stationary), 0),
	//						glm::vec4(0, 0, 0, 1));
	//glm::mat4 R = F
	
	stationary = (interpolation_constant*moving + stationary) /(interpolation_constant+1.0f);
	glm::vec3 v = glm::cross(moving, stationary);
	float c =     glm::dot(moving, stationary);
	if (abs(1.0f + c) < 0.001)
	{
		return glm::mat4(1);
	}
	glm::mat4 vx = glm::mat4(
		glm::vec4(0, -v.z, v.y, 0),
		glm::vec4(v.z, 0, -v.x, 0),
		glm::vec4(-v.y, v.x, 0, 0),
		glm::vec4(0, 0, 0, 0));
	glm::mat4 unit = glm::mat4(1);
	glm::mat4 R = unit + vx + vx * vx*(1.0f/(1.0f+c));
	return R;
}

void Snak::align_cameras()
{
	glm::mat4 head_trans = scn->get_shape(head_indx)->GetRot();//this->get_segment_rotation(head_indx + 1);
	//head_trans *= scn->get_shape(head_indx)->GetRot()
	glm::vec3 x_axis = glm::normalize(glm::vec3(head_trans*glm::vec4(1, 0, 0, 0)));
	glm::vec3 y_axis = glm::normalize(glm::vec3(head_trans*glm::vec4(0, 1, 0, 0)));
	glm::vec3 z_axis = glm::normalize(glm::vec3(head_trans*glm::vec4(0, 0, 1, 0)));
	glm::vec3 head_pose = get_head_pos();

	// first person camera
	// pos = head pose, forward -xaxis, up = yaxis
	Camera * cam = scn->get_camera(FirstPersonCamera);
	cam->set_camera_movable_params(head_pose+ y_axis, -x_axis, y_axis);
	cam->Pitch(30.0f);
	//third person camera
	// pos = head pose+ constant , forward -zaxis, up yaxis
	cam = scn->get_camera(ThirdPersonCamera);
	cam->set_camera_movable_params(head_pose +z_axis*10.0f, -z_axis, y_axis);
}
Snak::~Snak()
{
}
