#include "stdafx.h"
#include "NodeLoader.h"
#include <iostream>

NodeLoader::NodeLoader(std::string path, std::string name, double x, double y, double z)
{
	this->path = path;
	this->name = name;

	position = glm::vec3(x,y,z);
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

