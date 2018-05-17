#pragma once
#include "Turnable.h" 

enum openWay{RIGHT, LEFT};

class Door : public Turnable
{
public:
	Door(NodeLoader * door, openWay dahway);
	~Door();

	openWay dahway;

	void Open() override;
	void Close() override;
};

