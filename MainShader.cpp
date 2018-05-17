#include "stdafx.h"
#include "MainShader.h"
#include "vrlib/Texture.h"
#include "VrLib/gl/Vertex.h"
#include <VrLib\tien\components\TransformAttach.h>
#include <VrLib\tien\components\ModelRenderer.h>
#include "VrLib/tien/components/MeshRenderer.h"

vrlib::gl::Shader<Uniforms>* MainShader::mainShader()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/Bodemonderzoek/shaders/mixmap.vert", "data/Bodemonderzoek/shaders/mixmap.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texcoord", 2);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::modelMatrix, "modelMatrix");
	shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
	shader->registerUniform(Uniforms::viewMatrix, "viewMatrix");
	shader->registerUniform(Uniforms::s_mixmap, "s_mixmap");
	shader->registerUniform(Uniforms::s_texture0, "s_texture0");
	shader->registerUniform(Uniforms::s_texture1, "s_texture1");
	shader->registerUniform(Uniforms::diffuseColor, "diffuseColor");
	shader->registerUniform(Uniforms::textureFactor, "textureFactor");
	shader->use();
	shader->setUniform(Uniforms::s_mixmap, 0);
	shader->setUniform(Uniforms::s_texture0, 1);
	shader->setUniform(Uniforms::s_texture1, 2);

	mixmap_texture = vrlib::Texture::loadCached("data/Bodemonderzoek/textures/mixmap.png");
	texture0 = vrlib::Texture::loadCached("data/Bodemonderzoek/textures/GroundTiles/grassTile_C.png");
	texture1 = vrlib::Texture::loadCached("data/Bodemonderzoek/textures/GroundTiles/concrete_C.png");

	return shader;
}

void MainShader::mainDraw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	glDisable(GL_CULL_FACE);
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::diffuseColor, glm::vec3(1, 1, 1));
	shader->setUniform(Uniforms::textureFactor, 1.0f);

	mixmap_texture->bind();
	glActiveTexture(GL_TEXTURE1);
	texture0->bind();
	glActiveTexture(GL_TEXTURE2);
	texture1->bind();
	glActiveTexture(GL_TEXTURE0);

	//dit opbouwen moet natuurlijk niet ieder frame, maar voor 1 quad kan 't wel hier steeds
	std::vector<vrlib::gl::VertexP3N3T2> verts;
	vrlib::gl::VertexP3N3T2 vert;
	vrlib::gl::setN3(vert, glm::vec3(0, 1, 0));
	vrlib::gl::setP3(vert, glm::vec3(-100, 0, -100)); vrlib::gl::setT2(vert, glm::vec2(0, 0)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(-100, 0, 100)); vrlib::gl::setT2(vert, glm::vec2(0, 40)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(100, 0, 100)); vrlib::gl::setT2(vert, glm::vec2(40, 40)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(100, 0, -100)); vrlib::gl::setT2(vert, glm::vec2(40, 0)); verts.push_back(vert);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
