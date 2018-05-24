
#pragma once
#include "Uniforms.h"

#include <VrLib\Application.h>
#include <VrLib\tien\Tien.h>
#include <VrLib\HtcVive.h>
#include <VrLib\Device.h>
#include <glm\glm.hpp>
#include <string>
#include "NodeLoader.h"
#include "Turnable.h"
#include "Door.h"
#include "Manhole.h"
#include "MainShader.h"
#include "HandController.h"

using namespace vrlib::tien;
class Bodemonderzoek : public vrlib::Application{
	vrlib::Vive vive;
	vrlib::tien::Tien Engine;
	MainShader mainShader;

public:
	Bodemonderzoek();
	~Bodemonderzoek();

	vrlib::Texture* gridTexture;

	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
	virtual void latePreFrame() override;

	void loadScene();
	NodeLoader * searchNode(std::string searchTerm);
private:
	bool showDebug;
	std::vector<NodeLoader> nodes;

	HandController * rightHand;
	HandController * leftHand;

	Turnable * doorLeft;
	Turnable * doorRight;
	Turnable * manhole;
};