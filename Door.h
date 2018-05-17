#pragma once
#include "Turnable.h" 

class Door : public Turnable
{
public:
	Door(NodeLoader * door);
	~Door();

	void Open() override;
	void Close() override;
};

