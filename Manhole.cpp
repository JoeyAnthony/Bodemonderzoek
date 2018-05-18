#include "stdafx.h"
#include "Manhole.h"


Manhole::Manhole(NodeLoader * manhole)
{
	object = manhole;
	object->scale = glm::vec3(0.001,0.001,0.001);
}


Manhole::~Manhole()
{
}

void Manhole::Open()
{
	if (isOpen) return;
	object->rotation.x -= step;
	object->rotation.w = cos(object->rotation.x);

	object->position.y = sin(-object->rotation.x);
	object->position.z = cos(object->rotation.x);

	if (object->rotation.x <= -radiant) {
		isOpen = true;
	}

}

void Manhole::Close()
{
	if (!isOpen) return;
	object->rotation.x += step;
	object->rotation.w = cos(object->rotation.x);

	object->position.y = sin(-object->rotation.x);
	object->position.z = cos(object->rotation.x);

	if (object->rotation.x >= 0.0) {
		isOpen = false;
	}
}
