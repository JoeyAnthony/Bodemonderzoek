#pragma once
#include "stdafx.h"
#include <string>
#include <VrLib\tien\Tien.h>
#include <iostream>

using namespace vrlib::tien;
class NodeLoader
{
public:
	NodeLoader(std::string path, std::string name, glm::vec3 position = {0,0,0});
	~NodeLoader();
	void MoveForward();

	glm::quat rotation;
	glm::vec3 position, originPosition;
	glm::vec3 scale;
	std::string name, path;
	Node* node;
};

