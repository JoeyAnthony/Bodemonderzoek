#pragma once
#include <VrLib\HtcVive.h>
#include <VrLib\tien\Tien.h>
#include <VrLib\tien\Component.h>
#include "Turnable.h"

using namespace vrlib::tien;

class HandController : public vrlib::tien::components::Renderable
{
	class RayRenderer : public vrlib::tien::components::Renderable::RenderContext
	{
	public:
		void init() override;
		void frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix) override;
	};

private:
	float debugspeed = 0.3f;

public:
	vrlib::Vive::Controller controller;
	void initHand();
	void update(float elapsedTime, vrlib::tien::Scene& scene) override;
	void postUpdate(vrlib::tien::Scene& scene) override;

	glm::vec3 drawRay(glm::mat4 view, glm::mat4 proj);
	void checkTeleport(glm::mat4 data, Tien & engine, glm::mat4 view, glm::mat4 proj);
	bool checkInteractableItems(glm::mat4 data, Tien& engine, glm::mat4 view, glm::mat4 proj, std::vector<Interactable*> interactables);

	void drawDeferredPass() override;
	void drawForwardPass() override {};
	void drawShadowMap() override {};

	HandController(vrlib::Vive::Controller c);
	~HandController();

	vrlib::DigitalState actionButton;
	vrlib::tien::Node* teleportTarget = nullptr;
	Interactable* actionTarget = nullptr;
	glm::vec3 teleportTargetPosition;
	glm::vec3 closestHitPosition;
	double lastTeleportTime = 0;
	bool hasValidLocation = false;
	bool objectIsBusy = false;
};
