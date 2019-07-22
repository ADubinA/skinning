#include "snak.h"




Snak::Snak(int num_of_joints, Scene* scn)
{
	this->num_of_links = num_of_joints;
	this->head_indx = 0;
	this->tail_indx = 0;
	this->is_dead = 1;
	this->velocity = glm::vec3 (-1,0,0);
	this->scn = scn;
	this->speed = 0.01;
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
	Camera *cam3 = scn->get_camera(ThirdPersonCamera);
	Camera *cam1 = scn->get_camera(FirstPersonCamera);
	switch (direction)
	{
	case Up:
		this->velocity =glm::vec3( glm::rotate(ROTATION_SPEED, z_after_rotation)*head_rotation*glm::vec4(-1,0,0,0));
		scn->get_shape(this->head_indx)->myRotate(ROTATION_SPEED, z_after_rotation, -1);
		//TODO remove
		//scn->get_shape(this->head_indx+1)->myRotate(-1*ROTATION_SPEED, z_after_rotation, -1);
		cam3->RotateZ(-ROTATION_SPEED);
		cam1->Pitch(ROTATION_SPEED);
		

		break;

	case Down:
		this->velocity = glm::vec3(glm::rotate(-ROTATION_SPEED, z_after_rotation)*head_rotation*glm::vec4(-1, 0, 0, 0));
		scn->get_shape(this->head_indx)->myRotate(-ROTATION_SPEED, z_after_rotation, -1);

		//TODO remove
		//scn->get_shape(this->head_indx + 1)->myRotate(1 * ROTATION_SPEED, z_after_rotation, -1);

		cam3->RotateZ(ROTATION_SPEED);
		cam1->Pitch(-ROTATION_SPEED);

		break;

	case Left:
		this->velocity = glm::vec3(glm::rotate(ROTATION_SPEED, y_after_rotation)*head_rotation*glm::vec4(-1, 0, 0, 0));
		scn->get_shape(this->head_indx)->myRotate(ROTATION_SPEED, y_after_rotation, -1);

		//TODO remove
		//scn->get_shape(this->head_indx + 1)->myRotate(-1 * ROTATION_SPEED, y_after_rotation, -1);

		cam3->RotateY(ROTATION_SPEED);
		cam3->MoveRight(-1);
		cam1->RotateY(ROTATION_SPEED);
		
		break;

	case Right:
		this->velocity = glm::vec3(glm::rotate(-ROTATION_SPEED, y_after_rotation)*head_rotation*glm::vec4(-1, 0, 0, 0));
		scn->get_shape(this->head_indx)->myRotate(-ROTATION_SPEED, y_after_rotation, -1);
		//TODO remove
		//scn->get_shape(this->head_indx + 1)->myRotate(1 * ROTATION_SPEED, y_after_rotation, -1);
		cam3->RotateY(-ROTATION_SPEED);
		cam3->MoveRight(1);
		cam1->RotateY(-ROTATION_SPEED);

		break;

	case Forward:

		break;
	default:
		break;
	}
	int tmp_picked_shape = scn->get_picked_shape();
	scn->set_picked_shape(this->head_indx);

	scn->shapeTransformation(scn->xGlobalTranslate, speed*velocity.x);
	scn->shapeTransformation(scn->yGlobalTranslate, speed*velocity.y);
	scn->shapeTransformation(scn->zGlobalTranslate, speed*velocity.z);
	cam3->MoveRight(speed);
	cam1->MoveForward(speed);


	scn->set_picked_shape(tmp_picked_shape);
}

glm::vec3 Snak::get_head_pos()
{
	BoundingBox *head_box = scn->get_shape(this->head_indx)->mesh->bvh.box;
	return head_box->center - head_box->size;
}

Snak::~Snak()
{
}
