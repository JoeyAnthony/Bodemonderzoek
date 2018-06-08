#include "stdafx.h"
#include "Door.h"


Door::Door(NodeLoader * door, openWay dahway)
{
	object = door;
	this->dahway = dahway;

	if (dahway == RIGHT){
		object->rotation.y = radiant; 
	}
	else {
		object->rotation.y = -radiant;
	}

	object->rotation.w = cos(object->rotation.y);
	action = Action::TURN;
}


Door::~Door()
{
}

void Door::Open()
{
	if (isOpen) return;
	switch (dahway) {
	case RIGHT:
		object->rotation.y -= step;
		if (object->rotation.y <= 0.0) {
			isOpen = true;
			isDone = true;
		}
		break;
	case LEFT:
		object->rotation.y += step;
		if (object->rotation.y >= 0.0) {
			isOpen = true;
			isDone = true;
		}
		break;
	}
	object->rotation.w = cos(object->rotation.y);

}

void Door::Close()
{
	if (!isOpen) return;
	switch (dahway) {
		case RIGHT:
			object->rotation.y += step;
			if (object->rotation.y >= radiant) {
				isOpen = false;
				isDone = true;
			}
			break;
		case LEFT:
			object->rotation.y -= step;
			if (object->rotation.y <= -radiant) {
				isOpen = false;
				isDone = true;
			}
			break;
	}
	object->rotation.w = cos(object->rotation.y);
}
