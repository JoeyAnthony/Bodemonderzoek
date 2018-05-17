#include "stdafx.h"
#include "Turnable.h"


Turnable::Turnable()
{
}

Turnable::~Turnable()
{
}

void Turnable::Open()
{
}

void Turnable::Close()
{
}

void Turnable::TestOpenClose()
{
	if (isOpen) {
		Close();
	}
	else {
		Open();
	}
}
