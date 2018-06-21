#include "stdafx.h"

#include "MapApp.h"
#include "MainApp.h"
#include "Tablet.h"
#include <VrLib\Texture.h>
#include <VrLib\tien\components\Transform.h>

using namespace glm;
using namespace vrlib;
using namespace vrlib::tien;
using namespace vrlib::tien::components;
using namespace TabletGraphicsComonents;

MapApp::MapScroll::MapScroll(std::string map, float wordToMapScale, glm::vec2 centerWorld, TabletGraphicsObject* parent) : 
	TabletGraphicsObject(parent), texture(vrlib::Texture::loadCached(map)), wordToMapScale(wordToMapScale), centerWorld(centerWorld) {
	TabletGraphicsComonents::Details::ensureFontShaderInit();
}

void MapApp::MapScroll::draw(TabletGraphicsRenderInfo renderInfo) {
	using namespace TabletGraphicsComonents::Details;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisable(GL_CULL_FACE);

	const auto mapPos = (worldPos - centerWorld) / wordToMapScale + vec2(0.5f,0.5f);

	float mapZoomAdj = 1;
	if (mapZoom < 0) {
		mapZoomAdj = pow(2, -mapZoom);
	}
	else if (mapZoom > 0) {
		mapZoomAdj = 1.f / pow(2, mapZoom);
	}

	const auto xoffset = mapZoomAdj /2;
	const auto yoffset = (getGeometry().size.y / (float)getGeometry().size.x) * mapZoomAdj /2;

	fontShader->use();
	fontShader->setUniform(FontUniform::modelMatrix, renderInfo.modelViewMatrix);
	fontShader->setUniform(FontUniform::projectionMatrix, renderInfo.projectionMatrix);
	fontShader->setUniform(FontUniform::color, glm::vec4(1, 1, 1, 1));

	std::vector<vrlib::gl::VertexP2T2> verts;

	ivec2 position = { 0,0 };
	ivec2 size = getGeometry().size;
	
	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x, position.y),                   mapPos + glm::vec2(-xoffset, -yoffset)));
	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x + size.x, position.y),          mapPos + glm::vec2(+xoffset, -yoffset)));
	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x + size.x, position.y + size.y), mapPos + glm::vec2(+xoffset, +yoffset)));
	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x, position.y + size.y),          mapPos + glm::vec2(-xoffset, +yoffset)));

	texture->bind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vrlib::gl::setAttributes<vrlib::gl::VertexP2T2>(&verts[0]);

	glDrawArrays(GL_QUADS, 0, verts.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void MapApp::MapScroll::setWorldPos(glm::vec2 worldPos) {
	this->worldPos = worldPos;
}

void MapApp::MapScroll::zoomIn() {
	mapZoom = std::min(mapZoom + 1, 5);
}

void MapApp::MapScroll::zoomOut() {
	mapZoom = std::max(mapZoom - 1, -2);
}

MapApp::MapApp(Node* node) : node(node) {
	settings[Updateable] = true;
}

void MapApp::initalize() {
	background = new Square({}, {}, this);
	background->color = background->hoverColor = { 0.9,0.9,0.9 };
	background->setGeometry(getGeometry());
	baseMap = new MapScroll("data/BOdemonderzoek/textures/WorldMap.png", 200, { 0,0 }, this);
	baseMap->setGeometry({ {50,50},getGeometry().size - ivec2(100,190) });
	baseMap->setWorldPos({ 0,0 });
	cursor = new Square({}, {}, this);
	cursor->color = cursor->hoverColor = { 1,0,0 };
	cursor->setGeometry({ {1536 / 2 - 15, (2048 - 190 + 50) / 2 - 15}, {30,30} });
	mainMenuButton = new Button("Back", [tablet = tablet, &mainApp = mainApp] {tablet->setActiveApp(mainApp); }, this);
	mainMenuButton->setGeometry({ {50, 1920 - 100},{} });
	zoomIn = new Button(" +", [&baseMap = baseMap] {baseMap->zoomIn(); }, this);
	zoomIn->setGeometry({ {1080 - 50 - 60 - 80, 1920 - 100}, {60,60} });
	zoomOut = new Button(" -", [&baseMap = baseMap] {baseMap->zoomOut(); }, this);
	zoomOut->setGeometry({ { 1080 - 50 - 60, 1920 - 100 },{ 60,60 } });
}

bool MapApp::linkToApps() {
	if ((mainApp = tablet->getApp<MainApp>()) == nullptr) return false;
	return true;
}

void MapApp::update(float deltaMS) {
	vec3 pos = node->getComponent<components::Transform>()->position;
	baseMap->setWorldPos({ pos.z, pos.x });
}