#pragma once
#include <list>
#include <glm\common.hpp>
#include <functional>
#include <bitset>
#include <VrLib\Font.h>
#include <map>
#include <VrLib\gl\shader.h>
#include <VrLib\gl\FBO.h>

class Tablet;

struct TabletGraphicsRenderInfo {
	glm::ivec2 resolution;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewMatrix;
};

class TabletGraphicsObject {
public:
	struct Geometry {
		glm::ivec2 position;
		glm::ivec2 size;
	};
private:

	friend Tablet;

	// Object tree variables
	std::list<TabletGraphicsObject*> children;
	TabletGraphicsObject* parent;

	// Object tree methods
	void addChild(TabletGraphicsObject* child);
	void removeChild(TabletGraphicsObject* child);

	// State
	bool hadHover = false;
	Geometry geometry;
public:

	TabletGraphicsObject(TabletGraphicsObject* parent, bool _childerenShareMouseHover = true);
	virtual ~TabletGraphicsObject();

	enum Settings {
		Active = 0,
		Visable = 1,
		Clickable = 2,
		Hoverable = 3,
		Updateable = 4
	};
	using SettingsStore = typename std::bitset<5>;

	SettingsStore settings;

	const Geometry& getGeometry();
	virtual void setGeometry(Geometry g);
	virtual glm::ivec2 getMinimumSize();

	bool isHovering() { return hadHover; }

	// This variable is quite complex, if it is set to true when you hover over one of the children you can also hover over others
	// This is usefull for composition, for instance a buttons text and background can both react to the hover
	const bool childerenShareMouseHover;

	virtual void draw(TabletGraphicsRenderInfo renderInfo) {};
	virtual void onClick() {};
	virtual void onMouseEnter() {};
	virtual void onMouseLeave() {};
	virtual void update(float deltaMs) {};
};

class TabletApp : public TabletGraphicsObject {
protected:
	friend Tablet;

	Tablet * tablet = nullptr;
	TabletApp() : TabletGraphicsObject(nullptr, false) {};

public:

	virtual void initalize() {}; // When this method is called the tablet pointer is valid, but acces to other apps does not yet work
	virtual bool linkToApps() { return true; }; // When this method is called you can query the tablet for other apps if you need a pointer to them
	virtual void updateInactive(float deltaMs) {}; // This method is called once during every update for inactive apps, NOTE: children do not get updated
	virtual void onActivation() {}; // This method is called when this app becomes the active app
	virtual void onDeactivation() {}; // This method is called when this app becomes inactive after being the active app
};

namespace TabletGraphicsComonents {
	namespace Details {
		// The common shader that we use to render text and other textures

		enum class FontUniform
		{
			projectionMatrix,
			modelMatrix,
			s_texture,
			color,
		};
		extern vrlib::gl::Shader<FontUniform>* fontShader;

		void ensureFontShaderInit();
	}

	class Square : public TabletGraphicsObject {
	public:
		glm::vec3 color, hoverColor;

		Square(glm::vec3 _color, glm::vec3 _hoverColor, TabletGraphicsObject* parent);

		void draw(TabletGraphicsRenderInfo renderInfo) override;
	};

	class Text : public TabletGraphicsObject {
		static std::map<std::pair<std::string, float>, vrlib::TrueTypeFont*> fonts;
		vrlib::TrueTypeFont* font;
	public:

		glm::vec3 color, hoverColor;
		std::string text;
		const float height;

		Text(glm::vec3 color, glm::vec3 hoverColor, std::string text, std::string font, float heigth, TabletGraphicsObject* parent);

		void draw(TabletGraphicsRenderInfo renderInfo) override;
		glm::ivec2 getMinimumSize() override;
	};

	class Button : public TabletGraphicsObject {
		Square* background;
		Text* buttonText;
		std::function<void()> onTriggered;
	public:
		
		Button(std::string text, std::function<void()>&& func, TabletGraphicsObject* parent, glm::vec3 backgroundColor = { 255.f/255, 94.f/255, 102.f/255 }, glm::vec3 textColor = { 0,0,0 }, glm::vec3 backgroundHoverColor = { 213.f/255, 56.f/255, 61.f/255 }, glm::vec3 textHoverColor = { 0,0,0 });

		void setGeometry(Geometry g) override;
		glm::ivec2 getMinimumSize() override;

		void onClick() override;
	};

	class Texture : public TabletGraphicsObject {
	protected:
		vrlib::Texture* texture;
		Texture(TabletGraphicsObject* parent);
	public:
		Texture(vrlib::Texture* texture, TabletGraphicsObject* parent);

		void draw(TabletGraphicsRenderInfo renderInfo) override;
	};

	class Picture : public Texture {
	public:
		Picture(const std::string& image, TabletGraphicsObject* parent);
	};

	class FBO : public Texture {
	public:
		FBO(vrlib::gl::FBO* fbo, TabletGraphicsObject* parent);
	};
}