#pragma once
#include <vrlib/gl/shader.h>
#include <vrlib/Model.h>
#include "stdafx.h"

class VrModelLoader {

public:
	VrModelLoader(std::string path, vrlib::gl::Shader<Uniforms>* shader);
	~VrModelLoader();
	
	void Draw();
private:
	vrlib::Model* model;
	vrlib::ModelInstance* modelInstance; 
	vrlib::gl::Shader<Uniforms>* shader;
};