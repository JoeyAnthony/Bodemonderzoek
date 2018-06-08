#pragma once

#include "Tablet.h"

#include <glm\common.hpp>
#include <iostream>
#include <vector>

class CameraApp;

using namespace glm;
using namespace TabletGraphicsComonents;

class CameraApp;
class MapApp;

class MainApp : public TabletApp {
	Square* background;
	Text* title;
	Text* counter;

	std::vector<Button*> buttons;

	CameraApp* camApp;
	MapApp * mapApp;
public:
	

	MainApp() {};

	void initalize() override;
	bool linkToApps() override;

	void updatePhotoProgres(int taken, int total);
};