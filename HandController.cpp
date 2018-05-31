#include "stdafx.h"
#include "HandController.h"
#include <glm\gtc\matrix_transform.hpp>
#include <VrLib\math\Ray.h>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\intersect.hpp>

using namespace vrlib;
using namespace vrlib::tien;

void HandController::update(float elapsedTime, Scene & scene)
{
}

void HandController::postUpdate(Scene & scene)
{
}

glm::vec3 HandController::drawRay(glm::mat4 view, glm::mat4 proj)
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
	float length;
	if(hasValidLocation)glm::intersectRayPlane(rayOrigin, rayDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), length);
	else length = 10;

	glm::vec3 rayTarget{ rayOrigin + rayDir * length};

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3fv(glm::value_ptr(rayOrigin));
	glVertex3fv(glm::value_ptr(rayTarget));
	glEnd();
	return rayTarget;
}

void HandController::drawDeferredPass()
{

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

void HandController::checkTeleport(glm::mat4 data, Tien& engine, glm::mat4 view, glm::mat4 proj)
{
	teleportButton = controller.touchButton.getData();
	if (teleportButton == vrlib::DigitalState::TOGGLE_OFF && hasValidLocation)
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
		teleportTarget->getComponent<vrlib::tien::components::Renderable>()->visible = false;
	}

	if (teleportButton == vrlib::DigitalState::ON) 
	{
		std::vector<Node *> plane = engine.scene.findNodesWithName("plane");

		glm::mat4 wandMat = controller.transform.getData();
		vrlib::math::Ray pointer;
		pointer.mOrigin = glm::vec3(engine.scene.cameraNode->transform->globalTransform * wandMat * glm::vec4(0, 0, 0, 1));
		pointer.mDir = glm::normalize(pointer.mOrigin - glm::vec3(engine.scene.cameraNode->transform->globalTransform * wandMat * glm::vec4(0, 0, 1, 1)));
		vrlib::tien::Node* closestClickedNode = nullptr;
		glm::vec3 closestHitPosition;
		float closest = 100.0f;
		hasValidLocation = false;
		if (pointer.mDir.y < -0.2)
		{
			hasValidLocation = true;
			closestHitPosition = drawRay(view, proj);
		}


		/*engine.scene.castRay(pointer, [&, this](vrlib::tien::Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
		{
			bool collides = false;
			this->node->fortree([&collides, node](vrlib::tien::Node* n) { collides |= node == n; });
			if (collides)
				return true;

			if (hitFraction < closest && hitFraction > 0)
			{
				closest = hitFraction;
				closestClickedNode = node;
				closestHitPosition = hitPosition;
			}
			return true;
		}, false);
		*/
		if (hasValidLocation)
		{
			teleportTarget->getComponent<vrlib::tien::components::Renderable>()->visible = true;
			/*if (glm::abs(closestHitPosition.y) < 2.0f)
			{
				teleportTarget->transform->position = closestHitPosition;
				teleportTargetPosition = closestHitPosition;
			}
			else
			{*/
			glm::vec3 diff = closestHitPosition - pointer.mOrigin;
			float length = glm::length(diff);
			diff = glm::normalize(diff);
			teleportTargetPosition = pointer.mOrigin + length * diff;
			teleportTarget->transform->position = glm::vec3(teleportTargetPosition.x, 0, teleportTargetPosition.z);
			//}
		}
		else {
			teleportTarget->getComponent<vrlib::tien::components::Renderable>()->visible = false;
			teleportTarget->transform->position = glm::vec3(0, 0, 0);
		}
		
	}
}

void HandController::checkInteractableItems(glm::mat4 data, Tien& engine, glm::mat4 view, glm::mat4 proj, std::vector<Interactable> interactables) {
	vrlib::DigitalState button = controller.gripButton.getData();

	if (button == vrlib::DigitalState::TOGGLE_OFF && actionTarget) {
		switch (actionTarget->action) {
		case TURN:
			break;
		case TELEPORT:
			break;
		default:
			break;
		}
		actionTarget = nullptr;
	}

	if (button == vrlib::DigitalState::ON) {
		drawRay(view, proj);
		for (Interactable& inter : interactables) {

		}
	}

}
