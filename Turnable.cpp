#include "stdafx.h"
#include "Turnable.h"


Interactable::Interactable()
{
}

Interactable::~Interactable()
{
}

void Interactable::Open()
{
}

void Interactable::Close()
{
}

void Interactable::Teleport(glm::mat4 data, Tien& engine)
{
}

void Interactable::OpenClose()
{
	if (isOpen) {
		Close();
	}
	else {
		Open();
	}
}
