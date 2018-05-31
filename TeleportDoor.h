#pragma once
#include "Turnable.h"
class TeleportDoor : public Interactable
{
public:
	TeleportDoor(NodeLoader * tellyDoor, glm::vec3 tellyLocation);
	~TeleportDoor();

	void Teleport(glm::mat4 data ,Tien& engine) override;
private:
	glm::vec3 location;
};

