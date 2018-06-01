// Bodemonderzoek.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Bodemonderzoek.h"

#include <VrLib\Log.h>
#include <VrLib\json.hpp>
#include <fstream>

#include <VrLib\tien\components\TransformAttach.h>
#include <VrLib\tien\components\ModelRenderer.h>
#include <VrLib\tien\components\Camera.h>


using vrlib::logger;
using namespace vrlib::tien;

Bodemonderzoek::Bodemonderzoek()
{
}

Bodemonderzoek::~Bodemonderzoek()
{
}

void Bodemonderzoek::init()
{
	logger << "Initialized" << logger.newline;

	nodes.push_back(new Manhole(new NodeLoader("data/Bodemonderzoek/models/Manhole/Manhole.fbx", "Manhole", glm::vec3(2, 0, 2))));

	mainShader.mainShader();

	vive.init();

	leftHand = new HandController(vive.controllers[0]);
	rightHand = new HandController(vive.controllers[1]);

	Engine.init();

	loadScene();

	Engine.start();
}

void Bodemonderzoek::draw(const glm::mat4 & projectionMatrix, const glm::mat4 & modelViewMatrix)
{
	for (Interactable* nl : nodes) {
		nl->object->node->transform = new components::Transform(nl->object->position, nl->object->rotation, nl->object->scale);
	}

	Engine.render(projectionMatrix, modelViewMatrix);

	//leftHand->drawRay(modelViewMatrix, projectionMatrix);
	//rightHand->drawRay(modelViewMatrix, projectionMatrix);

	mainShader.mainDraw(projectionMatrix, modelViewMatrix, 0, 0);
	rightHand->checkTeleport(vive.hmd.getData(), Engine, modelViewMatrix, projectionMatrix);
	rightHand->checkInteractableItems(vive.hmd.getData(), Engine, modelViewMatrix, projectionMatrix, nodes);
}

void Bodemonderzoek::preFrame(double frameTime, double totalTime)
{
	Engine.update((float)frameTime / 1000.0f);
}

void Bodemonderzoek::latePreFrame()
{
}

void Bodemonderzoek::loadScene()
{
	Engine.scene.reset();

	std::ifstream str = std::ifstream("../Development/Applications/Bodemonderzoek/data/Scenes/TabletWorld.json");
	json scene = json::parse(str);
	Engine.scene.fromJson(scene["scene"], scene, [](auto) {return nullptr; });

	Node* leftHand = new Node("LeftHand", &Engine.scene);
	leftHand->addComponent(new components::TransformAttach(vive.controllers[0].transform));
	leftHand->addComponent(new components::Transform(glm::vec3(0, 0, 0)));
	leftHand->addComponent(this->leftHand);
	leftHand->addComponent(new components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_6.obj"));

	Node* rightHand = new Node("RightHand", &Engine.scene);
	rightHand->addComponent(new components::TransformAttach(vive.controllers[1].transform));
	rightHand->addComponent(new components::Transform(glm::vec3(0, 0, 0)));
	rightHand->addComponent(this->rightHand);
	rightHand->addComponent(new components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_6.obj"));

	this->rightHand->teleportTarget = new vrlib::tien::Node("HandRightIndicator", &Engine.scene);
	this->rightHand->teleportTarget->addComponent(new vrlib::tien::components::Transform(glm::vec3(), glm::quat(), glm::vec3(0.01f, 0.01f, 0.01f)));
	this->rightHand->teleportTarget->addComponent(new vrlib::tien::components::ModelRenderer("data/virtueelpd/models/idle.fbx"));
	this->rightHand->teleportTarget->getComponent<vrlib::tien::components::ModelRenderer>()->visible = false;

	Engine.scene.cameraNode = Engine.scene.findNodeWithName("Camera");

	for (Interactable* nl : nodes) {
		Node* node = new Node(nl->object->name, &Engine.scene);
		node->addComponent(new components::Transform(nl->object->position, nl->object->rotation, nl->object->scale));
		node->addComponent(new components::ModelRenderer(nl->object->path));
		nl->object->node = node;
	}
 }

NodeLoader* Bodemonderzoek::searchNode(std::string searchTerm)
{
	NodeLoader *searchNode = nullptr;
	for (Interactable* nl : nodes) {
		if (nl->object->name._Equal(searchTerm)) {
			searchNode = nl->object;
			break;
		}
	}
	return searchNode;
}
