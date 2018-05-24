#include "stdafx.h"
#include "HandController.h"
#include <glm\gtc\matrix_transform.hpp>
#include <VrLib\math\Ray.h>
#include <glm\gtx\quaternion.hpp>

using namespace vrlib;
using namespace vrlib::tien;

void HandController::update(float elapsedTime, Scene & scene)
{
}


void HandController::postUpdate(Scene & scene)
{
}

void HandController::drawRay(glm::mat4 view, glm::mat4 proj)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(proj));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(view));
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 0, 0, 1);
	glDisable(GL_BLEND);
	glLineWidth(10.0f);

	glm::vec3 rayOrigin{ node->transform->transform * glm::vec4(0, 0, 0, 1) };
	//glm::mat4 rotmat = glm::rotate(node->transform->transform, glm::radians(-60.f), glm::vec3(1, 0, 0));
	//glm::vec3 rayFront{rotmat * glm::vec4(0, 0, -1, 1)};
	glm::vec3 rayFront{ node->transform->transform * glm::vec4(0, 0, -1, 1) };
	glm::vec3 rayDir = glm::vec3(rayFront - rayOrigin);
	//glm::vec3 rayDir = glm::vec3(glm::vec4(glm::normalize(rayFront - rayOrigin), 1.f) * glm::rotate(glm::mat4(), -45.f, glm::vec3(1, 0, 0)));

	glm::vec3 rayTarget{ rayOrigin + rayDir * 20.f };
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3fv(glm::value_ptr(rayOrigin));
	glVertex3fv(glm::value_ptr(rayTarget));
	glEnd();
}

void HandController::drawDeferredPass() {


}

HandController::HandController(vrlib::Vive::Controller c)
{
	controller = c;
}


HandController::~HandController()
{
}

void HandController::RayRenderer::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
{
	//drawRay(projectionMatrix, viewMatrix);
	//renderContextForward;
}

void HandController::checkTeleport(glm::mat4 data, Tien& engine, double totalTime)
{
	teleportButton = controller.touchButton.getData();
	if (teleportButton == vrlib::DigitalState::TOGGLE_OFF)
	{
		teleportTarget->getComponent<vrlib::tien::components::Renderable>()->visible = false;
		if (lastTeleportTime > totalTime)
		{
			glm::vec3 target = teleportTarget->transform->getGlobalPosition();
			if (glm::abs(target.y) < 0.1)
			{
				glm::vec3 headPosition(data * glm::vec4(0, 0, 0, 1));
				target.x -= headPosition.x;
				target.z -= headPosition.z;

				engine.scene.cameraNode->transform->position = glm::vec3(target.x, 0, target.z);

				glm::vec3 position((data * glm::vec4(0, 0, 0, 1)) + (engine.scene.cameraNode->transform->globalTransform * glm::vec4(0, 0, 0, 1)));
			}
		}		
	}

	if (teleportButton == vrlib::DigitalState::TOGGLE_ON) {
		lastTeleportTime = totalTime + 500;
	}

	if (teleportButton == vrlib::DigitalState::ON)
	{
		glm::mat4 wandMat = controller.transform.getData();
		vrlib::math::Ray pointer;
		pointer.mOrigin = glm::vec3(engine.scene.cameraNode->transform->globalTransform * wandMat * glm::vec4(0, 0, 0, 1));
		pointer.mDir = glm::normalize(pointer.mOrigin - glm::vec3(engine.scene.cameraNode->transform->globalTransform * wandMat * glm::vec4(0, 0, 1, 1)));
		vrlib::tien::Node* closestClickedNode = nullptr;
		glm::vec3 closestHitPosition;
		float closest = 99999999.0f;

		engine.scene.castRay(pointer, [&, this](vrlib::tien::Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
		{
			bool collides = false;
			node->fortree([&collides, node](vrlib::tien::Node* n) { collides |= node == n; });

			if (hitFraction < closest && hitFraction > 0)
			{
				closest = hitFraction;
				closestClickedNode = node;
				closestHitPosition = hitPosition;
			}
			return true;
		}, false);

		if (closestClickedNode)
		{
			//teleportTarget->getComponent<vrlib::tien::components::Renderable>()->visible = true;
			if (glm::abs(closestHitPosition.y) < 0.1)
			{
				teleportTarget->transform->position = closestHitPosition;
				teleportTargetPosition = closestHitPosition;
			}
			else
			{
				glm::vec3 diff = closestHitPosition - pointer.mOrigin;
				float length = glm::length(diff);
				diff = glm::normalize(diff);
				if (length > 0.25f)
					length -= 0.25f;
				teleportTargetPosition = pointer.mOrigin + length * diff;
				teleportTarget->transform->position = glm::vec3(teleportTargetPosition.x, 0, teleportTargetPosition.z);
			}
		}
		else
			teleportTarget->getComponent<vrlib::tien::components::Renderable>()->visible = false;
	}
}
