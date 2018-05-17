#include "stdafx.h"
#include "Manhole.h"


Manhole::Manhole(NodeLoader * manhole)
{
	object = manhole;
}


Manhole::~Manhole()
{
}

void Manhole::Open()
{
	object->rotation.x -= step;
	object->rotation.w = cos(object->rotation.x);
	if (object->rotation.x <= -radiant) {
		isOpen = true;
	}

}

void Manhole::Close()
{
	object->rotation.x += step;
	object->rotation.w = cos(object->rotation.x);
	if (object->rotation.x >= 0.0) {
		isOpen = false;
	}
}
