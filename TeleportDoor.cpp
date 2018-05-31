#include "stdafx.h"
#include "TeleportDoor.h"


TeleportDoor::TeleportDoor(NodeLoader * tellyDoor, glm::vec3 tellyLocation)
{
	object = tellyDoor;
	location = tellyLocation;
	action = Action::TELEPORT;
}


TeleportDoor::~TeleportDoor()
{
}

void TeleportDoor::Teleport(glm::mat4 data, Tien& engine)
{
	glm::vec3 headPosition(data * glm::vec4(0, 0, 0, 1));
	glm::vec3 target;

	target.x = location.x - headPosition.x;
	target.z = location.z - headPosition.z;

	engine.scene.cameraNode->transform->position = glm::vec3(target.x, 0, target.z);
	glm::vec3 position((data * glm::vec4(0, 0, 0, 1)) + (engine.scene.cameraNode->transform->globalTransform * glm::vec4(0, 0, 0, 1)));
}
