#pragma once
#include <VrLib\tien\Tien.h>
#include <VrLib/tien/Component.h>	
#include "OcclusionQuery.h"
#include <VrLib/Singleton.h>
#include <VrLib/tien/components/ModelRenderer.h>

class CameraApp;

class VisibilityTester : public vrlib::tien::Component
{
private:
	OcclusionQuery query;
	glm::vec3 minbounds;
	glm::vec3 maxbounds;

public:
	CameraApp * camApp;
	float treshold;
	vrlib::tien::components::ModelRenderer *modelRenderer;
	
	/*
	* Returns the the amount of the object that covers the screen with a value between 0 and 1
	*/
	float isVisible();
	vrlib::tien::Node* OcclusionDraw(glm::mat4 view, glm::mat4 proj);

	OcclusionQuery getQuery();

	VisibilityTester(float tresh, vrlib::tien::components::ModelRenderer* model);
	VisibilityTester(float tresh, vrlib::tien::components::ModelRenderer* model, CameraApp* camapp);
	~VisibilityTester();
};

