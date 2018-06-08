#include "stdafx.h"
#include "Tablet.h"

#include <glm\gtx\intersect.hpp>

using vrlib::gl::FBO;
using std::initializer_list;

using namespace vrlib;
using namespace vrlib::tien;
using namespace glm;

void Tablet::updateMesh()
{
	vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
	vio.setData(indices.size(), &indices[0], GL_STATIC_DRAW);

	if (!vao)
		vao = new gl::VAO(&vbo);
	vao->bind();
	vio.bind();
	vao->unBind();
}

void Tablet::updateInput() {

	// We set the input values to valid values, so if we have to abort these are guaranteed to be acceptable
	m_screenPos = { 0,0 };
	m_screenPosInBounds = false;
	if (node == nullptr) return;

	// We take the inverse of our transfrom to convert the 'pointer' position to tablet space
	const mat4 toTabletSpaceMat = inverse(node->transform->transform);

	// We get the position and front of the 'pointer' and convert them to tablet space
	const vec3 pointerPos = vec3(toTabletSpaceMat * (m_pointer.getData() * vec4(0, 0, 0, 1)));
	const vec3 pointerFront = vec3(toTabletSpaceMat * (m_pointer.getData() * vec4(0, 0, -1, 1)));
	const vec3 pointerDir = pointerPos - pointerFront;

	// We use a ray plane intersect to calculate our intersection point
	// @TODO: replace with TIEN's comperable code (Math plane.rayIntersect)
	float intersectionDistance;
	bool intersectSucces = intersectRayPlane(pointerPos, pointerDir, {}, { 0,0,-1 }, intersectionDistance);
	if (intersectSucces == false) return;
	auto intercectionPosition = pointerPos + intersectionDistance * pointerDir;

	// We convert our intersection point to pixel cordinates
	ivec2 intercectionPosition2{ m_planePosToPixelCoordMat * vec4(intercectionPosition, 1) };

	// We set the screen pos and do bounds checks, we also indicate wether the intersection point was within the bounds
	m_screenPos = min(max(ivec2(intercectionPosition2), { 0,0 }), m_resolution);
	m_screenPosInBounds = ivec2(intercectionPosition2) == m_screenPos;
}

Tablet::updateResult Tablet::updateGraphicsObject(TabletGraphicsObject* obj, float deltaMs, ivec2 mousePos, bool inBounds, bool active, bool mouseDown) {
	updateResult result{ false, false };
	bool useHover = true;

	// We adjust the mouse position, and check wether we are within the bounds
	mousePos -= obj->getGeometry().position;
	inBounds = inBounds ? mousePos.x >= 0 && mousePos.x < obj->getGeometry().size.x && mousePos.y >= 0 && mousePos.y < obj->getGeometry().size.y : false;

	active = active && obj->settings[TabletGraphicsObject::Active];

	if (active && obj->settings[TabletGraphicsObject::Updateable]) obj->update(deltaMs);

	// We loop over all our children
	for (auto& child : obj->children) {
		auto childResult = updateGraphicsObject(child, deltaMs, mousePos, inBounds && useHover, active, mouseDown && !result.tookClick);

		// We check if the child took the mouse, and adjust our state. We do this to prevent the mouse clicking or hovering over multiple elements
		if (result.tookClick == false && (childResult.tookHover || childResult.tookClick)) {
			useHover = obj->childerenShareMouseHover;
			result = childResult;
		}
	}

	// We check wether we are hovering over this object, and call the nececary methods
	if (active && useHover && obj->settings[TabletGraphicsObject::Hoverable]) {
		if (obj->hadHover && !inBounds) {
			obj->onMouseLeave();
		}
		else if (!obj->hadHover && inBounds) {
			obj->onMouseEnter();
		}
		obj->hadHover = inBounds;
	}
	else if (obj->hadHover) {
		obj->onMouseLeave();
		obj->hadHover = false;
	}

	// We check wether this object was clicked, and set the nececary objects
	if (!result.tookClick && active && inBounds && mouseDown && obj->settings[TabletGraphicsObject::Clickable]) {
		result.tookClick = true;

		if (m_mouseDownObject) throw "there can only be one mouse down object";

		m_mouseDownObject = obj;
	}

	return result;
}

void Tablet::updateApps(float deltaMs) {
	// We check if the trigger has been pressed
	bool mouseDown = m_trigger.getData() == DigitalState::TOGGLE_ON;

	// If the trigger has been released, and if so handle the completed click
	if (m_trigger.getData() == DigitalState::TOGGLE_OFF && m_mouseDownObject) {
		m_mouseDownObject->onClick();
		m_mouseDownObject = nullptr;
		mouseDown = false;
	}

	// We recursivly update the active scene
	updateGraphicsObject(activeApp, deltaMs, m_screenPos, m_screenPosInBounds, true, mouseDown);

	// We call the updateInactive function for all inactive apps
	for (auto& app : apps)
		if (app != activeApp)
			app->updateInactive(deltaMs);
}

void Tablet::drawGraphicsObject(TabletGraphicsObject* obj, glm::mat4 transform) {

	// We update the transfrom with the position so the opengl code can be in local space
	transform[3][0] += obj->getGeometry().position.x;
	transform[3][1] += obj->getGeometry().position.y;

	// We load the adjusted matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(transform));
	m_activeTransfrom = transform;

	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);

	// We draw the object
	obj->draw({ m_resolution,m_pixelToTexCoordMat,transform });

	// We draw all children that are visable
	for (auto child : obj->children)
		if (child->settings[TabletGraphicsObject::Visable])
			drawGraphicsObject(child, transform);
}

void Tablet::updateScreen() {
	clear();

	// Setup the fbo as a render target
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, m_fbo.getWidth(), m_fbo.getHeight());
	m_fbo.bind();

	// Setup to allow easy graphics code in app
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(m_pixelToTexCoordMat)); // this matrix wil convert pixel coordinates to texture coordinates
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);

	// Draw the app
	drawGraphicsObject(activeApp, { 1,0,0,0,
		0,1,0,0,
		0,0,0,0,
		0,0,0,1 });

	// Remove the fbo as render target
	m_fbo.unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void Tablet::update(float elapsedTime, Scene& scene) {
	updateInput();
	updateApps(elapsedTime);
	updateScreen();
}

void Tablet::postUpdate(Scene& scene) {

}

void Tablet::clear(vec4 clearColor) {

	// Setup the fbo as a render target
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, m_fbo.getWidth(), m_fbo.getHeight());
	m_fbo.bind();

	// We clear the fbo with the given clear color ( default = {0,0,0,1} )
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);

	// Remove the fbo as render target
	m_fbo.unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

Tablet::Tablet(ivec2 resolution, float size, const PositionalDevice& pointer, const DigitalDevice& trigger, initializer_list<TabletApp*> apps) :
	m_resolution(resolution), m_size(size), m_withToHeightRatio(((float)m_resolution.y) / m_resolution.x),
	m_pointer(pointer), m_trigger(trigger), m_fbo(m_resolution.x, m_resolution.y), m_fboTexture(&m_fbo), 
	
	// We construct a matrix to convert our plane coordinates to pixel coordinates
	// Syntax: [a,b] is the inclusive range between a and b. '=>' is the transformation we want. 'f' is the function that does the conversion
	// x | [ -0.5 * size, 0.5 * size] => [0, resolution.x] | f(x) = x * (resolution.x / size) + resolution.x/2
	// Same for y, but there the ratio is also a factor
	// y | [ -0.5 * size * ratio, 0.5 * size * ratio] => [0, resolution.y] | f(y) = y * ((resolution.y / size) / ratio) + resolution.y/2 
	m_planePosToPixelCoordMat(m_resolution.x / m_size, 0, 0, 0,
							  0, (m_resolution.y / m_withToHeightRatio) / m_size, 0, 0,
							  0, 0, 0, 0,
							  m_resolution.x / 2.f, m_resolution.y / 2.f, 0, 0),

	// We contruct a matrix to convert our pixel coordinates to the fbo's texture coordinates
	// x | [0, resolution.x] => [-1, 1] | f(x) = x * (1/resolution.x) * 2 - 1
	// y | [0, resolution.y] => [-1, 1] | f(y) = y * (1/resolution.y) * 2 - 1
	m_pixelToTexCoordMat( (1.f / m_resolution.x) * 2,0,0,0,
						  0,(1.f / m_resolution.y) * 2,0,0,
						  0,0,1,0,
						  -1,-1,0,1 ),
	apps(apps)
	{
	
	// We setup the fbo as the mesh's texture
	material.texture = &m_fboTexture;
	material.normalmap = nullptr;

	// We clear the fbo
	clear();


	// We settup the mesh
	// @NOTE: this has mostly been copied from NetworkEngine/PanelComponent.cpp/PanelComponent::PanelComponent(...)

	// Setup Normal, tangent, and bi-tangent
	vrlib::gl::VertexP3N2B2T2T2 v;
	vrlib::gl::setN3(v, glm::vec3(0, 0, 1));
	vrlib::gl::setTan3(v, glm::vec3(1, 0, 0));
	vrlib::gl::setBiTan3(v, glm::vec3(0, 1, 0));

	// We create a vector with the plane / mesh's x and y size
	vec2 sizevec{ m_size, m_size * m_withToHeightRatio };

	// We create the plane's four vertecies
	vrlib::gl::setP3(v, glm::vec3(-sizevec.x / 2.0f, -sizevec.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(0, 0));			vertices.push_back(v);
	vrlib::gl::setP3(v, glm::vec3(sizevec.x / 2.0f, -sizevec.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(1, 0));			vertices.push_back(v);
	vrlib::gl::setP3(v, glm::vec3(sizevec.x / 2.0f, sizevec.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(1, 1));			vertices.push_back(v);
	vrlib::gl::setP3(v, glm::vec3(-sizevec.x / 2.0f, sizevec.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(0, 1));			vertices.push_back(v);

	// We create the mesh from the indicies by forming two triangles from the 4 vertecies
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	// Renderer setup
	vao = nullptr;
	updateMesh();

	// We set the render context 
	renderContextForward = ShadelessForwardRenderContext::getInstance();

	// We check apps, and ensure proper initialization
	if (apps.size() == 0) throw "you need to pass at leat one app";
	for (auto& app : this->apps) {
		app->tablet = this;
		app->geometry.position = { 0,0 };
		app->geometry.size = m_resolution;
		app->initalize();
	}

	for (auto& app : this->apps)
		if (app->linkToApps() == false)
			throw "Unable to link apps";

	activeApp = this->apps[0];
	activeApp->onActivation();
}

void Tablet::setActiveApp(TabletApp* app) {
	activeApp->onDeactivation();
	activeApp = app;
	activeApp->onActivation();
}

void Tablet::drawForwardPass()
{
	// We get the context and ensure our shader is used
	ShadelessForwardRenderContext* context = dynamic_cast<ShadelessForwardRenderContext*>(renderContextForward);
	context->renderShader->use();

	// We get and set the transform
	components::Transform* t = node->getComponent<vrlib::tien::components::Transform>();
	context->renderShader->setUniform(ShadelessForwardRenderContext::RenderUniform::modelMatrix, t->globalTransform);

	// We bind our texture (the screen fbo)
	material.texture->bind();

	// We draw the mesh
	if (vao)
	{
		vao->bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		Renderer::drawCalls++;
		vao->unBind();
	}
}

void Tablet::ShadelessForwardRenderContext::init()
{
	// We load the shader
	renderShader = new vrlib::gl::Shader<RenderUniform>("data/TabletTestProject/Shaders/simple.vert", "data/TabletTestProject/Shaders/simple.frag");

	// Set the atributes
	renderShader->bindAttributeLocation("a_position", 0);
	renderShader->bindAttributeLocation("a_texture", 4);

	// Link
	renderShader->link();

	// And register the uniforms
	renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
	renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
	renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
	renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
}

void Tablet::ShadelessForwardRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
{
	renderShader->use();
	renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
	renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
}