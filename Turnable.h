#pragma once
#include "NodeLoader.h"

class Turnable
{
public:
	Turnable();
	~Turnable();

	NodeLoader * object;
	bool isOpen = false;

	float radiant = 0.7f;
	float step = 0.01f;

	virtual void Open();
	virtual void Close();
	void TestOpenClose();
};

