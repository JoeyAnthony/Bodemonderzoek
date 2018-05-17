
#pragma once
#include "Uniforms.h"

#include <VrLib/Application.h>
#include <VrLib/tien/Tien.h>
#include <VrLib/HtcVive.h>
#include <VrLib/Device.h>
#include <glm/glm.hpp>

class Bodemonderzoek : public vrlib::Application
{
	vrlib::Vive vive;
	vrlib::tien::Tien Engine;

public:
	Bodemonderzoek();
	~Bodemonderzoek();

	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
	virtual void latePreFrame() override;

	void loadScene();
private:
	bool showDebug;

	vrlib::Texture* mixmap_texture;
	vrlib::Texture* texture0;
	vrlib::Texture* texture1;

	enum class Uniforms
	{
		modelMatrix,
		projectionMatrix,
		viewMatrix,
		s_mixmap,
		s_texture0,
		s_texture1,
		diffuseColor,
		textureFactor
	};

	vrlib::gl::Shader<Uniforms>* shader;
};