#pragma once
#include "NodeLoader.h"

class Turnable
{
public:
	Turnable();
	~Turnable();

	NodeLoader * object;
	bool isOpen = false;

	float radiant = 0.7;
	float step = 0.01;

	virtual void Open();
	virtual void Close();
	void TestOpenClose();
};

