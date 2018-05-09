#pragma once
#include "stdafx.h"
#include <string>

class NodeLoader
{
public:
	NodeLoader(std::string path, std::string name, double x = 0, double y = 0, double z = 0);
	~NodeLoader();
	double x = 0, y = 0, z = 0;
	std::string name, path;
};

