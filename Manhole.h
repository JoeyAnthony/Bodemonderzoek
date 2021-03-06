#pragma once
#include "Turnable.h"

class Manhole : public Interactable
{
public:
	Manhole(NodeLoader * manhole);
	~Manhole();

	void Open() override;
	void Close() override;
};

