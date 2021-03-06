// Bodemonderzoek.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Bodemonderzoek.h"

#include <VrLib\Log.h>
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

	//Action Models
	nodes.push_back(new Manhole(new NodeLoader("data/Bodemonderzoek/models/Manhole/Manhole.fbx", "Manhole", glm::vec3(-20, 1, -35))));
	
	//Teleport Models
	nodes.push_back(new TeleportDoor(new NodeLoader("data/Bodemonderzoek/models/emergencyDoor/emergencyDoor.fbx",
		"DoorToBasement", glm::vec3(39, 0, -28.4)), glm::vec3(3.3, 0, -124.9)));

	nodes.push_back(new TeleportDoor(new NodeLoader("data/Bodemonderzoek/models/emergencyDoor/emergencyDoor.fbx",
		"DoorToOutside", glm::vec3(3.3, 0, -124.9)), glm::vec3(38, 0, -28.4)));

	//Set Scale
	searchNode("Manhole")->scale = glm::vec3(0.002, 0.002, 0.002);
	searchNode("DoorToOutside")->scale = glm::vec3(1, 2.7, 2);
	searchNode("DoorToBasement")->scale = glm::vec3(1, 2, 1.1);

	//Rotate
	searchNode("DoorToBasement")->rotation = glm::quat(0, 0, 1, 0);
	
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

	//MOET DIT ER IN?
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

	//Load Scene -MOET DE NAAM VERANDERT WORDEN?-
	std::ifstream str = std::ifstream("../Development/Applications/Bodemonderzoek/data/Scenes/TabletWorld.json");
	json scene = json::parse(str);
	Engine.scene.fromJson(scene["scene"], scene, [](auto) {return nullptr; });


	//Terrain Renderer 
	float scale = 1;
	terrain = new Terrain();
	terrain->setSize(200, 200);
	Node* ground = new Node("Terrain", &Engine.scene);
	ground->addComponent(new components::Transform(glm::vec3(-100, 0, -100)));
	auto terrainRenderer = new components::TerrainRenderer(terrain);

	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/stone_tile_diffuse.jpg",
		"data/Bodemonderzoek/textures/GroundTiles/stone_tile_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/stone_tile_mask.png");

	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/asphalt_diffuse.png",
		"data/Bodemonderzoek/textures/GroundTiles/asphalt_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/asphalt_mask.png");

	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/grass_diffuse.png",
		"data/Bodemonderzoek/textures/GroundTiles/grass_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/grass_mask.png");

	terrainRenderer->addMaterialLayer("data/Bodemonderzoek/textures/GroundTiles/concrete_diffuse.png",
		"data/Bodemonderzoek/textures/GroundTiles/concrete_normal.png",
		"data/Bodemonderzoek/textures/GroundTiles/concrete_mask.png");

	ground->addComponent(terrainRenderer);
	ground->addComponent(new components::TerrainCollider(ground));

	//KAN DIT ER UIT?
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

	//Add Tablet 
	Node* leftHand = new Node("LeftHand", &Engine.scene);
	auto a = [this](const Scene& scene, const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix, Node* cameraNode, int renderId) {this->Engine.renderer.render(scene, projectionMatrix, modelViewMatrix, cameraNode, renderId); };
	CameraApp* camApp = new CameraApp(leftHand, a);

	leftHand->addComponent(new components::Transform(glm::vec3(-2, 0, 0)));
	leftHand->addComponent(new components::TransformAttach(vive.controllers[0].transform));
	Tablet* tablet = new Tablet({ 1536,2048 }, 0.25, vive.controllers[0].transform, vive.controllers[1].transform, vive.controllers[1].triggerButton, { new MainApp(), camApp, new MapApp(leftHand) });
	leftHand->addComponent(tablet);
	leftHand->addComponent(new components::ModelRenderer("data/Bodemonderzoek/models/tipad/TiPad.obj"));

	//The 3 Cracks Inside Repair Hall
	vrlib::tien::components::ModelRenderer* m = new components::ModelRenderer("data/Bodemonderzoek/models/crack/crack.obj");
	Node* crack1 = new Node("crack1", &Engine.scene);
	crack1->addComponent(new components::Transform(glm::vec3(-9, 0.01, -28.5), glm::quat() , glm::vec3(0.3, 0.01, 0.3)));
	crack1->addComponent(m);
	crack1->addComponent(new VisibilityTester(0.15f, m, camApp));

	vrlib::tien::components::ModelRenderer* m2 = new components::ModelRenderer("data/Bodemonderzoek/models/crack/crack.obj");
	Node* crack2 = new Node("crack2", &Engine.scene);
	crack2->addComponent(new components::Transform(glm::vec3(-3.5, 0.01, -26), glm::quat(), glm::vec3(0.3, 0.01, 0.3)));
	crack2->addComponent(m2);
	crack2->addComponent(new VisibilityTester(0.15f, m2, camApp));

	vrlib::tien::components::ModelRenderer* m3 = new components::ModelRenderer("data/Bodemonderzoek/models/crack/crack.obj");
	Node* crack3 = new Node("crack3", &Engine.scene);
	crack3->addComponent(new components::Transform(glm::vec3(-10, 0.01, -27), glm::quat(), glm::vec3(0.3, 0.01, 0.3)));
	crack3->addComponent(m3);
	crack3->addComponent(new VisibilityTester(0.15f, m3, camApp));

	//Basement Generator
	vrlib::tien::components::ModelRenderer* m4 = new components::ModelRenderer("data/Bodemonderzoek/models/diesel_generator_fbx/diesel_generator.fbx");
	Node* generator = new Node("generator", &Engine.scene);
	generator->addComponent(new components::Transform(glm::vec3(21, 0, -133.735), glm::quat(), glm::vec3(1, 1, 1)));
	generator->addComponent(m4);
	generator->addComponent(new VisibilityTester(0.15f, m4, camApp));

	//The Left Side Tank
	vrlib::tien::components::ModelRenderer* m5 = new components::ModelRenderer("data/Bodemonderzoek/models/diesel_generator_fbx/diesel_generator.fbx");
	Node* generator2 = new Node("generator2", &Engine.scene);
	generator2->addComponent(new components::Transform(glm::vec3(-14.231, 0, -31.13), glm::quat(), glm::vec3(1, 1, 1)));
	generator2->addComponent(m5);
	generator2->addComponent(new VisibilityTester(0.15f, m5, camApp));

	//The Crack Under The Manhole
	vrlib::tien::components::ModelRenderer* m6 = new components::ModelRenderer("data/Bodemonderzoek/models/crack/crack.obj");
	Node* crack4 = new Node("crack4", &Engine.scene);
	crack4->addComponent(new components::Transform(glm::vec3(-20, 0.01, -35), glm::quat(), glm::vec3(1, 0.01, 1)));
	crack4->addComponent(m6);
	crack4->addComponent(new VisibilityTester(0.15f, m6, camApp));
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
