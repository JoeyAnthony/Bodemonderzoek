#include "VrModelLoader.h"
#include "Uniforms.h"
#include <VrLib/models/AssimpModel.h>
#include <VrLib/Device.h>
#include <VrLib/HtcVive.h>
#include "stdafx.h"

VrModelLoader::VrModelLoader(std::string path, vrlib::gl::Shader<Uniforms>* shader)
{
	this->shader = shader;
	model = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2B4B4>(path);
	modelInstance = model->getInstance();
	((vrlib::State*)modelInstance)->playAnimation("Armature|ArmatureAction.001");
}

VrModelLoader::~VrModelLoader()
{
}

void VrModelLoader::Draw()
{
	modelInstance->draw([this](const glm::mat4 &modelMatrix)
	{
		shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	},
		[this](const vrlib::Material &material)
	{
		if (material.texture)
		{
			shader->setUniform(Uniforms::textureFactor, 1.0f);
			material.texture->bind();
		}
		else
		{
			shader->setUniform(Uniforms::textureFactor, 0.0f);
			shader->setUniform(Uniforms::diffuseColor, material.color.diffuse);
		}
		return true;
	});
}
