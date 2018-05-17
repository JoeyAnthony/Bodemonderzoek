#include "stdafx.h"
#include "Door.h"


Door::Door(NodeLoader * door)
{
	object = door;
}


Door::~Door()
{
}

void Door::Open()
{
	object->rotation.y += 0.01;
	object->rotation.w = cos(object->rotation.y);
	if (object->rotation.y >= 0.7) {
		isOpen = true;
	}

}

void Door::Close()
{
	object->rotation.y -= 0.01;
	object->rotation.w = cos(object->rotation.y);
	if (object->rotation.y <= 0.0) {
		isOpen = false;
	}
}
