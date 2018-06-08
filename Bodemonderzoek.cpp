// Bodemonderzoek.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Bodemonderzoek.h"

#include <VrLib\Log.h>
#include <VrLib\json.hpp>
#include <fstream>

#include <VrLib\tien\components\TransformAttach.h>
#include <VrLib\tien\components\ModelRenderer.h>
#include <VrLib/tien/components/TerrainRenderer.h>
#include "VrLib/tien/Terrain.h"
#include "VrLib/tien/components/TerrainCollider.h"

#include "tablet/Tablet.h"
#include "tablet/CameraApp.h"
#include "tablet/MainApp.h"
#include "tablet/MapApp.h"




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

	nodes.push_back(new Manhole(new NodeLoader("data/Models/Characters/Bodies/paul/paul.dae", "Manhole", glm::vec3(-20, 1.5, 0))));
	nodes.push_back(new TeleportDoor(new NodeLoader("data/Bodemonderzoek/models/emergencyDoor/emergencyDoor.fbx", 
	"DoorToAttic", glm::vec3(1, 0, 1)), glm::vec3(3.3, 0, -124.9)));
	nodes.push_back(new TeleportDoor(new NodeLoader("data/Bodemonderzoek/models/emergencyDoor/emergencyDoor.fbx", 
	"DoorToOutside", glm::vec3(3.3, 0, -124.9)), glm::vec3(1, 0, 1)));

	//examples on how to change scaling and rotation, quats work a bit wonky though
	searchNode("DoorToOutside")->scale = glm::vec3(1, 2.7, 2);

	searchNode("DoorToAttic")->rotation = glm::quat(0,0,0,0);
	searchNode("DoorToOutside")->rotation = glm::quat(0, 0, 0, 0);
	
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
	rightHand->drawRay(modelViewMatrix, projectionMatrix);
	if (!rightHand->checkInteractableItems(vive.hmd.getData(), Engine, modelViewMatrix, projectionMatrix, nodes)) {
		rightHand->checkTeleport(vive.hmd.getData(), Engine, modelViewMatrix, projectionMatrix);
	}
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

	//Terrain Renderer 
	float scale = 1;
	terrain = new Terrain(); //"data/TienTest/biker/Textures/heightmap.png", 50.0f

	terrain->setSize(200, 200);

	Node* ground = new Node("Terrain", &Engine.scene);
	ground->addComponent(new components::Transform(glm::vec3(-100, 0, -100)));//glm::vec3()-terrain->getWidth() / 2.0f * scale, -15, -terrain->getHeight() / 2.0f* scale), glm::quat(), glm::vec3(scale, scale, scale)
	auto terrainRenderer = new components::TerrainRenderer(terrain);

	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/grass_diffuse.png",
		"data/Bodemonderzoek/textures/GroundTiles/grass_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/grass_mask.png");
	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/asphalt_diffuse.png",
		"data/Bodemonderzoek/textures/GroundTiles/asphalt_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/asphalt_mask.png");
	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/dirt_diffuse.png",
		"data/Bodemonderzoek/textures/GroundTiles/dirt_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/dirt_mask.png");
	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/concrete_diffuse.png",
		"data/Bodemonderzoek/textures/GroundTiles/concrete_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/concrete_mask.png");

	ground->addComponent(terrainRenderer);
	ground->addComponent(new components::TerrainCollider(ground));

	//Node* leftHand = new Node("LeftHand", &Engine.scene);
	//leftHand->addComponent(new components::TransformAttach(vive.controllers[0].transform));
	//leftHand->addComponent(new components::Transform(glm::vec3(0, 0, 0)));
	//leftHand->addComponent(this->leftHand);
	//leftHand->addComponent(new components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_6.obj"));

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

	//tablet stuff
	Node* leftHand = new Node("LeftHand", &Engine.scene);
	auto a = [this](const Scene& scene, const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix, Node* cameraNode, int renderId) {this->Engine.renderer.render(scene, projectionMatrix, modelViewMatrix, cameraNode, renderId); };
	CameraApp* camApp = new CameraApp(leftHand, a);

	leftHand->addComponent(new components::Transform(glm::vec3(-2, 0, 0)));
	leftHand->addComponent(new components::TransformAttach(vive.controllers[0].transform));
	Tablet* tablet = new Tablet({ 1536,2048 }, 0.25, vive.controllers[1].transform, vive.controllers[1].triggerButton, { new MainApp(), camApp, new MapApp(leftHand) });
	leftHand->addComponent(tablet);
	leftHand->addComponent(new components::ModelRenderer("data/Bodemonderzoek/models/tipad/TiPad.obj"));

	Node* testobj = new Node("Testobj", &Engine.scene);
	testobj->addComponent(new components::Transform(glm::vec3(0.5, 0, -2)));
	vrlib::tien::components::ModelRenderer* m = new components::ModelRenderer("data/Models/Environment/Building Blocks/Dungeon/wall_1.fbx");
	testobj->addComponent(m);
	testobj->addComponent(new VisibilityTester(0.15f, m, camApp));
	testobj->transform->setGlobalRotation(glm::quat({ 0, glm::radians(90.0f), 0 }));

	Node* testobj2 = new Node("Testobj2", &Engine.scene);
	testobj2->addComponent(new components::Transform(glm::vec3(-0.5, 0, -2)));
	vrlib::tien::components::ModelRenderer* m2 = new components::ModelRenderer("data/Models/Environment/Building Blocks/Dungeon/wall_1.fbx");
	testobj2->addComponent(m2);
	testobj2->addComponent(new VisibilityTester(0.15f, m, camApp));
	testobj2->transform->setGlobalRotation(glm::quat({ 0, glm::radians(90.0f), 0 }));
 }

NodeLoader* Bodemonderzoek::searchNode(std::string searchTerm)
{
	NodeLoader *searchNode = nullptr;
	for (Interactable* nl : nodes) {
		if (nl->object->name._Equal(searchTerm)) {
			return searchNode = nl->object;
		}
	}
	return searchNode;
}
