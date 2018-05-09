#include "stdafx.h"
#include "NodeLoader.h"

NodeLoader::NodeLoader(std::string path, std::string name, double x, double y, double z)
{
	this->path = path;
	this->name = name;
	this->x = x;
	this->y = y;
	this->z = z;
}


NodeLoader::~NodeLoader()
{
}

