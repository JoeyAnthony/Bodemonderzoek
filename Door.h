#pragma once
#include "Turnable.h" 

enum openWay{RIGHT, LEFT};

class Door : public Interactable
{
public:
	Door(NodeLoader * door, openWay dahway);
	~Door();

	openWay dahway;

	void Open() override;
	void Close() override;
};

