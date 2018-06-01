#pragma once
#include "NodeLoader.h"

enum Action {
	TURN, TELEPORT
};

class Interactable
{
public:
	Interactable();
	~Interactable();

	NodeLoader * object;
	Action action;

	bool isOpen = false;
	bool isDone = false;

	float radiant = 0.7f;
	float step = 0.01f;

	virtual void Open();
	virtual void Close();
	virtual void Teleport(glm::mat4 data, Tien& engine);
	void OpenClose();
};

