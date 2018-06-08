#include "stdafx.h"
#include "TabletGraphics.h"

#include <glm\common.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <VrLib\gl\FBO.h>
#include <VrLib\gl\shader.h>
#include "Tablet.h"

using namespace glm;

TabletGraphicsObject::TabletGraphicsObject(TabletGraphicsObject* _parent, bool _childerenShareMouseHover) : 
	parent(_parent), childerenShareMouseHover(_childerenShareMouseHover) 
{
	if (parent) parent->addChild(this);
	settings[Active] = true;
	settings[Visable] = true;
}

TabletGraphicsObject::~TabletGraphicsObject() {
	if (parent) parent->removeChild(this);
	for (auto* child : children)
		delete child;
}

const TabletGraphicsObject::Geometry& TabletGraphicsObject::getGeometry() {
	return geometry;
}

void TabletGraphicsObject::setGeometry(TabletGraphicsObject::Geometry g) {
	geometry = { g.position, max(getMinimumSize(), g.size) };
}

ivec2 TabletGraphicsObject::getMinimumSize() {
	return { 0,0 };
}


void TabletGraphicsObject::addChild(TabletGraphicsObject* child) {
	children.push_back(child);
}

void TabletGraphicsObject::removeChild(TabletGraphicsObject* child) {
	children.remove(child);
}

namespace TabletGraphicsComonents {
	namespace Details {
		vrlib::gl::Shader<FontUniform>* fontShader;

		void ensureFontShaderInit() {
			if (!fontShader)
			{
				fontShader = new vrlib::gl::Shader<FontUniform>("data/TabletTestProject/Shaders/font.vert", "data/TabletTestProject/Shaders/font.frag");
				fontShader->bindAttributeLocation("a_position", 0);
				fontShader->bindAttributeLocation("a_color", 1);
				fontShader->link();
				fontShader->registerUniform(FontUniform::projectionMatrix, "projectionMatrix");
				fontShader->registerUniform(FontUniform::modelMatrix, "modelMatrix");
				fontShader->registerUniform(FontUniform::s_texture, "s_texture");
				fontShader->registerUniform(FontUniform::color, "color");
				fontShader->use();
				fontShader->setUniform(FontUniform::s_texture, 0);
				fontShader->setUniform(FontUniform::color, glm::vec4(1,1,1,1));
			}
		}
	}
		
	std::map<std::pair<std::string, float>, vrlib::TrueTypeFont*> Text::fonts;

	Square::Square(vec3 _color, vec3 _hoverColor, TabletGraphicsObject* parent) : 
		TabletGraphicsObject(parent), color(_color), hoverColor(_hoverColor) {
		settings[Hoverable] = true;
	}



	void Square::draw(TabletGraphicsRenderInfo renderInfo) {
		glBegin(GL_QUADS); 

		if (isHovering()) glColor3f(hoverColor.r, hoverColor.g, hoverColor.b);
		else glColor3f(color.r, color.g, color.b );

		glVertex2fv(value_ptr(vec2(0, 0)));
		glVertex2fv(value_ptr(vec2(getGeometry().size.x, 0)));
		glVertex2fv(value_ptr(vec2(getGeometry().size.x, getGeometry().size.y)));
		glVertex2fv(value_ptr(vec2(0, getGeometry().size.y)));

		glEnd();
	}

	Text::Text(vec3 color, vec3 hoverColor, std::string text, std::string fontName, float height, TabletGraphicsObject* parent) : 
		TabletGraphicsObject(parent), color(color), hoverColor(hoverColor), text(text), height(height) {

		Details::ensureFontShaderInit();

		

		if (fonts.find(std::pair<std::string, float>(fontName, height)) == fonts.end())
		{
			vrlib::TrueTypeFont* f = new vrlib::TrueTypeFont(fontName, height);
			fonts[std::pair<std::string, float>(fontName, height)] = f;
		}

		font = fonts[std::pair<std::string, float>(fontName, height)];
		if (!font || !font->fileData)
			throw "Unable To Load Font";

		
	}

	void Text::draw(TabletGraphicsRenderInfo renderInfo) {
		using namespace Details;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		fontShader->use();
		
		//We invert the y-axis since font assumes a clasic y-axis, and we do some spacing
		renderInfo.projectionMatrix[1][1] *= -1;
		renderInfo.projectionMatrix[3][1] *= -1;
		renderInfo.modelViewMatrix[3][1] = renderInfo.resolution.y - renderInfo.modelViewMatrix[3][1] - height/4;

		fontShader->setUniform(FontUniform::projectionMatrix, renderInfo.projectionMatrix);
		fontShader->setUniform(FontUniform::modelMatrix, renderInfo.modelViewMatrix);
		fontShader->setUniform(FontUniform::color, glm::vec4(color, 1.0f));
		font->drawText<vrlib::gl::VertexP2T2>(text);

		glDisable(GL_BLEND);
	}

	ivec2 Text::getMinimumSize() {
		return { font->textlen(text), height };
	}

	Button::Button(std::string text, std::function<void()>&& func, TabletGraphicsObject* parent, vec3 backgroundColor, vec3 textColor, vec3 backgroundHoverColor, vec3 textHoverColor) :
		TabletGraphicsObject(parent), onTriggered(func)
	{
		background = new Square(backgroundColor, backgroundHoverColor, this);
		buttonText = new Text(textColor, textHoverColor, text, "Arial", 60, this);
		settings[Clickable] = true;
	}

	void Button::onClick() {
		onTriggered();
	}

	void Button::setGeometry(TabletGraphicsObject::Geometry g) {
		TabletGraphicsObject::setGeometry(g);
		buttonText->setGeometry({ { 0,0 }, getGeometry().size });
		background->setGeometry({ { 0,0 }, getGeometry().size });
	}

	ivec2 Button::getMinimumSize() {
		return buttonText->getMinimumSize();
	}

	Texture::Texture(TabletGraphicsObject* parent) : TabletGraphicsObject(parent) {
		Details::ensureFontShaderInit();
	}

	Texture::Texture(vrlib::Texture * texture, TabletGraphicsObject* parent) : TabletGraphicsObject(parent), texture(texture) {
		Details::ensureFontShaderInit();
	}

	void Texture::draw(TabletGraphicsRenderInfo renderInfo) {
		using namespace Details;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisable(GL_CULL_FACE);


		fontShader->use();
		fontShader->setUniform(FontUniform::modelMatrix, renderInfo.modelViewMatrix);
		fontShader->setUniform(FontUniform::projectionMatrix, renderInfo.projectionMatrix);
		fontShader->setUniform(FontUniform::color, glm::vec4(1, 1, 1, 1));

		std::vector<vrlib::gl::VertexP2T2> verts;

		ivec2 position = { 0,0 }; //getGeometry().position;
		ivec2 size = getGeometry().size;

		verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x, position.y), glm::vec2(0, 0)));
		verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x + size.x, position.y), glm::vec2(1, 0)));
		verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x + size.x, position.y + size.y), glm::vec2(1, 1)));
		verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x, position.y + size.y), glm::vec2(0, 1)));


		texture->bind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		vrlib::gl::setAttributes<vrlib::gl::VertexP2T2>(&verts[0]);

		glDrawArrays(GL_QUADS, 0, verts.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	Picture::Picture(const std::string& image, TabletGraphicsObject* parent) : Texture(parent) {
		texture = vrlib::Texture::loadCached(image);
	}

	FBO::FBO(vrlib::gl::FBO* fbo, TabletGraphicsObject* parent) : Texture(parent) {
		texture = new Tablet::FboToTexture(fbo);
	}
}