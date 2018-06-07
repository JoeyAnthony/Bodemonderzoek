#include "stdafx.h"
#include "NodeLoader.h"
#include <iostream>

NodeLoader::NodeLoader(std::string path, std::string name, glm::vec3 position)
{
	this->path = path;
	this->name = name;

	this->position = position;
	originPosition = position;
	rotation = glm::quat(0,0,0,0);
	scale = glm::vec3(1,1,1);
}

NodeLoader::~NodeLoader()
{
}

void NodeLoader::MoveForward()
{
	position.x += 0.1;
}

