#include "snak.h"




snak::snak()
{
}

void snak::createSnake(int num_of_joints, Scene* scn)
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



	BoundingBox* bodybox = scn->get_shape(body_index)->mesh->bvh.box;
	BoundingBox* edgebox = scn->get_shape(edge_index)->mesh->bvh.box; 
	scn->set_picked_shape(edge_index);
	scn->shapeTransformation(scn->xGlobalTranslate, edgebox->static_center.x - bodybox->size.x);
	scn->shapeTransformation(scn->yGlobalTranslate, edgebox->static_center.y);
	scn->shapeTransformation(scn->zGlobalTranslate, edgebox->static_center.z);
	for (int i = 0; i < num_of_joints; i++) {
		//addShapeCopy(body_index, edge_index + i - 1, QUADS);
		scn->addShapeCopy(body_index, -1, 5);
		scn->set_picked_shape(scn->get_num_of_shapes() - 1);
		scn->shapeTransformation(scn->xGlobalTranslate, bodybox->static_center.x + bodybox->size.x);
		scn->shapeTransformation(scn->yGlobalTranslate, bodybox->static_center.y);
		scn->shapeTransformation(scn->zGlobalTranslate, bodybox->static_center.z);
		scn->set_chain_parents(scn->get_picked_shape(),  scn->get_picked_shape() - 1);
	}


	scn->addShape(tail, -1, 5);
	scn->set_picked_shape(scn->get_num_of_shapes() - 1);
	//shapeTransformation(zLocalRotate, 180);
	scn->shapeTransformation(scn->xGlobalTranslate, edgebox->static_center.x + bodybox->size.x);
	//shapeTransformation(xLocalTranslate,  -2*bodybox->size.x);
	scn->shapeTransformation(scn->yGlobalTranslate, edgebox->static_center.y);
	scn->shapeTransformation(scn->zGlobalTranslate, edgebox->static_center.z);
	scn->set_chain_parents(scn->get_picked_shape(), scn->get_picked_shape() - 1);


}

void snak::move(glm::vec3 direction)
{
}

snak::~snak()
{
}
