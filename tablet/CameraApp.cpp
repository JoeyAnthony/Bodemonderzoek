#include "stdafx.h"
#include "CameraApp.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "MainApp.h"

using namespace vrlib::tien;
using namespace glm;
using namespace std::string_literals;

void CameraApp::initalize()
{
	settings[Updateable] = true;
	auto fbosize = std::min(getGeometry().size.x, getGeometry().size.y);
	fboRes = { fbosize, fbosize };
	fbo = new vrlib::gl::FBO(fboRes.x, fboRes.y, true, 1);

	background = new Square({}, {}, this);
	background->color = background->hoverColor = { 0.9,0.9,0.9 };
	background->setGeometry(getGeometry());
	mainMenuButton = new Button("Back", [tablet = tablet, &mainApp = mainApp] {tablet->setActiveApp(mainApp); }, this);
	mainMenuButton->setGeometry({ { 50, 1920 - 100 },{} });
	cameraFeed = new TabletGraphicsComonents::FBO(fbo, this);
	cameraFeed->setGeometry({ {50,(getGeometry().size.y - fboRes.y)/2 + 50},fboRes - ivec2(100,100) });
	feedbackBG = new Square({}, {}, this);
	feedbackBG->color = feedbackBG->hoverColor = background->color;
	feedback = new Text({}, {}, "feedback", "Arial", 50, this);
	feedback->setGeometry({ {cameraFeed->getGeometry().position.x + cameraFeed->getGeometry().size.x / 2 - feedback->getMinimumSize().x / 2 , cameraFeed->getGeometry().position.y + cameraFeed->getGeometry().size.y / 2 - 40},{} });
	feedbackBG->setGeometry(feedback->getGeometry());
	photoButton = new Button("Take Photo", [camapp = this]() {camapp->savePhoto(); }, this);
	photoButton->setGeometry({ {50, cameraFeed->getGeometry().position.y - 100}, {} });

	//set mat
	perspective = glm::perspective(glm::radians(fov), (float)fboRes.y / fboRes.x, 0.01f, 1000.0f);
	model = translate(glm::mat4(), camTabletOffset);
	addRouteNode();
}

void CameraApp::update(float deltaMS)
{
	if (hasUpdated == false) {
		hasUpdated = true;
		mainApp->updatePhotoProgres(pollutedObjectsFound.size(), occlusionObjects.size());
	}

	model = translate(glm::mat4(), camTabletOffset);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, fboRes.x, fboRes.y);
	fbo->bind();

	func(tabletHand->getScene(), perspective, model, tabletHand, 0);
	checkObjects();

	fbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	feedbackTimer -= deltaMS;
	std::cout << "timer: " << feedbackTimer << std::endl;
	if (feedbackTimer < 0 && (feedback->settings[Visable] || feedbackBG->settings[Visable])) {
		feedback->settings[Visable] = feedbackBG->settings[Visable] = false;
	}
}

void CameraApp::updateInactive(float deltaMS) {
	if (hasUpdated == false) {
		hasUpdated = true;
		mainApp->updatePhotoProgres(pollutedObjectsFound.size(), occlusionObjects.size());
	}
}

void CameraApp::addRouteNode()
{
	pdfexport.addRouteNode({ tabletHand->transform->position.x, tabletHand->transform->position.z }, false);
}

void CameraApp::setFeedback(std::string s) {
	feedback->settings[Visable] = feedbackBG->settings[Visable] = true;
	feedbackTimer = 3;
	feedback->text = s;
	feedback->setGeometry({ { cameraFeed->getGeometry().position.x + cameraFeed->getGeometry().size.x / 2 - feedback->getMinimumSize().x / 2 , cameraFeed->getGeometry().position.y + cameraFeed->getGeometry().size.y / 2 - 40 },{} });
	feedbackBG->setGeometry({ feedback->getGeometry().position - ivec2(10,10), feedback->getGeometry().size + ivec2(20,20) });
}

/*
* Saves all polluted nodes in the screen in pollutedObjInScreen
* saves node with the largest amount of rendered samples and null if no polluted object was seen
*/
void CameraApp::checkObjects()
{
	VisibilityTester* largest = nullptr;
	Node* largestNode = nullptr;
	pollutedObjInScreen.clear();
	for (VisibilityTester* objs : occlusionObjects)
	{
		Node* node = objs->OcclusionDraw(inverse(tabletHand->transform->globalTransform) * model, perspective);
		//checks if object is visible
		if (objs->isVisible() > 0.0f)
		{
			pollutedObjInScreen.push_back(node);

			if (!largest){
				largest = objs;
				largestNode = node;
			}
			else if (objs->getQuery().lastRenderedSamples() > largest->getQuery().lastRenderedSamples()){
				largest = objs;
				largestNode = node;
			}
		}
	}
	largestPollutedObjInScreen = largestNode;
}

void CameraApp::addOcclusionObject(Node* obj)
{
	VisibilityTester* vis = obj->getComponent<VisibilityTester>();
	if (vis != nullptr)
		occlusionObjects.push_back(vis);
}

void CameraApp::addOcclusionObject(VisibilityTester * obj)
{
	occlusionObjects.push_back(obj);
}

const std::vector<PollutedObjData> CameraApp::pollutedObjsList()
{
	return pollutedObjectsFound;
}

const glm::vec2 CameraApp::getRes()
{
	return fboRes;
}

void CameraApp::savePhoto()
{
	if (!largestPollutedObjInScreen && pollutedObjInScreen.size() < 1) {
		setFeedback("No polution source in photo, maybe move a bit closer?");
		return;
	}
	//checks if node is already photographed
	for (PollutedObjData pdata : pollutedObjectsFound) {
		if (pdata.node == largestPollutedObjInScreen) {
			setFeedback("You have already taken a photo of this polution source");
			return;
		}
	}

	setFeedback("Photo was taken succesfully");
	PollutedObjData data{ largestPollutedObjInScreen, vrlib::gl::FBO(fboRes.x, fboRes.y, true, 1) };

	data.photo.bind();
	fbo->use();
	data.photo.unbind();

	pollutedObjectsFound.push_back(data);
	mainApp->updatePhotoProgres(pollutedObjectsFound.size(), occlusionObjects.size());
	std::string path = "data/Bodemonderzoek/photos/"s + std::to_string(imagecount++) + ".jpg";
	fbo->saveAsFile(path);

	pdfexport.addPhoto(path, { 595.276f / 2, 841.89f / 2 }, 0);
	addRouteNode();
	
}

bool CameraApp::linkToApps() {
	if ((mainApp = tablet->getApp<MainApp>()) == nullptr) return false;
	return true;
}

void CameraApp::exportPhotos()
{
	pdfexport.generateReport("Report");
}

CameraApp::CameraApp(vrlib::tien::Node* hand) {
	tabletHand = hand;
}

CameraApp::CameraApp(vrlib::tien::Node * hand, std::function<void(const vrlib::tien::Scene&, const glm::mat4&, const glm::mat4&, vrlib::tien::Node*, int)> func) : CameraApp(hand) {
	this->func = func;
}


CameraApp::~CameraApp()
{
}
