#pragma once
#include "NodeLoader.h"

class Turnable
{
public:
	Turnable();
	~Turnable();

	NodeLoader * object;
	bool isOpen = false;

	virtual void Open();
	virtual void Close();
	void TestOpenClose();
};

