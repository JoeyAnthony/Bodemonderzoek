#pragma once
#include <glm/mat4x4.hpp>
#include "../Tutorials/RenderWithShader/RenderWithShader.h"
#include "Uniforms.h"

class MainShader
{
public:
	virtual vrlib::gl::Shader<Uniforms>* mainShader();
	virtual void mainDraw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);

	vrlib::gl::Shader<Uniforms>* shader;

private:
	bool showDebug;

	vrlib::Texture* mixmap_texture;
	vrlib::Texture* texture0;
	vrlib::Texture* texture1;
	vrlib::Texture* texture2;
};
