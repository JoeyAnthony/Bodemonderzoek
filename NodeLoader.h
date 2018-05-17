#pragma once
#include "stdafx.h"
#include <string>
#include <VrLib\tien\Tien.h>
#include <iostream>

using namespace vrlib::tien;
class NodeLoader
{
public:
	NodeLoader(std::string path, std::string name, double x = 0, double y = 0, double z = 0);
	~NodeLoader();
	void MoveForward();

	glm::quat rotation;
	glm::vec3 position;
	std::string name, path;
	Node* node;
};

