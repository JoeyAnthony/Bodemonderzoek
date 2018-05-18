// Bodemonderzoek.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Bodemonderzoek.h"

#include <VrLib\Log.h>
#include <VrLib\json.hpp>
#include <fstream>

#include <VrLib\tien\components\TransformAttach.h>
#include <VrLib\tien\components\ModelRenderer.h>

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
	
	nodes.push_back({ "data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_6.obj", "Righthand", 0, 2, -2 });
	nodes.push_back({ "data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_6.obj", "Lefthand", 0, 2, -2 });
	nodes.push_back({ "data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_6.obj", "Lefthand", 0, 2, -2 });
	doorLeft = new Door(&nodes[0], LEFT);
	manhole = new Manhole(&nodes[1]);
	doorRight = new Door(&nodes[2], RIGHT);

	mainShader.mainShader();

	Engine.init();

	loadScene();

	Engine.start();
}

void Bodemonderzoek::draw(const glm::mat4 & projectionMatrix, const glm::mat4 & modelViewMatrix)
{
	doorLeft->Close();
	doorRight->Open();
	manhole->TestOpenClose();
	for (NodeLoader& nl : nodes) {
		nl.node->transform = new components::Transform(nl.position, nl.rotation);
	}
	Engine.render(projectionMatrix, modelViewMatrix);

	mainShader.mainDraw(projectionMatrix, modelViewMatrix);
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

	for (NodeLoader& nl : nodes) {
		Node* node = new Node(nl.name, &Engine.scene);
		node->addComponent(new components::Transform(nl.position, nl.rotation));
		node->addComponent(new components::ModelRenderer(nl.path));
		node->addComponent(new components::TransformAttach(vive.controllers[1].transform));
		nl.node = node;
	}
 }

NodeLoader* Bodemonderzoek::searchNode(std::string searchTerm)
{
	NodeLoader *searchNode = nullptr;
	for (NodeLoader nl : nodes) {
		if (nl.name._Equal(searchTerm)) {
			searchNode = &nl;
			break;
		}
	}
	return searchNode;
}
