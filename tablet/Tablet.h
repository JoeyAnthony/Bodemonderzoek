#pragma once

#include "TabletGraphics.h"
#include <VrLib\tien\components\MeshRenderer.h>
#include <VrLib/gl/Vertex.h>
#include <vrlib/gl/VIO.h>
#include <vrlib/gl/VAO.h>
#include <vrlib/gl/VBO.h>
#include <VrLib\gl\FBO.h>
#include <VrLib\Device.h>
#include <VrLib\Texture.h>
#include <VrLib\tien\Tien.h>
#include <initializer_list>

class Tablet :  public vrlib::tien::components::MeshRenderer::Mesh, public vrlib::tien::components::Renderable {

	// Our custom render context that allows us to render the tablet with custom shaders
	class ShadelessForwardRenderContext : public vrlib::tien::components::Renderable::RenderContext, public vrlib::Singleton<ShadelessForwardRenderContext>
	{
	public:
		enum class RenderUniform
		{
			modelMatrix,
			projectionMatrix,
			viewMatrix,
			s_texture,
		};
		vrlib::gl::Shader<RenderUniform>* renderShader;
		vrlib::Texture* defaultNormalMap;
		virtual void init() override;
		virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
	};

	// Mesh render variables and methods
	void updateMesh();
	vrlib::gl::VBO<vrlib::gl::VertexP3N2B2T2T2> vbo;
	vrlib::gl::VIO<unsigned int> vio;
	vrlib::gl::VAO* vao;

public: // This class is public to allow easy acces from TabletGraphics
	// We use this class to convert our FBO to a texture for the mesh
	// @NOTE: this class originated in NetworkEngine/PanelComponent.h/PanelComponent
	// @TODO: maybe put this class in TIEN?
	class FboToTexture : public vrlib::Texture
	{
	public:
		vrlib::gl::FBO* fbo;
		FboToTexture(vrlib::gl::FBO* fbo) : fbo(fbo), vrlib::Texture(nullptr)
		{
		}

		inline void bind() override
		{
			fbo->use();
		}

	};
public:
	// Resolution related variables
	// m_size gives us the size of the x-axis and we use m_withToHeightRato -- wich is calculated by res.y/res.x -- to determine the size of the y-axis
	const glm::ivec2 m_resolution;
	const float m_size;
	const float m_withToHeightRatio;

private:
	
	// FBO related variables
	vrlib::gl::FBO m_fbo;
	FboToTexture m_fboTexture;

	// Constant matrixes to handle conversions
	const glm::mat4 m_pixelToTexCoordMat;
	const glm::mat4 m_planePosToPixelCoordMat;
	glm::mat4 m_activeTransfrom;

	// The input devices
	const vrlib::PositionalDevice& m_pointer;
	const vrlib::DigitalDevice& m_trigger;

	// The runtime input values
	glm::ivec2 m_screenPos;
	bool m_screenPosInBounds;
	TabletGraphicsObject* m_mouseDownObject = nullptr;

	struct updateResult {
		bool tookHover;
		bool tookClick;
	};

	void updateInput();
	updateResult updateGraphicsObject(TabletGraphicsObject* obj, float deltaMs, glm::ivec2 mousePos, bool inBounds, bool active, bool mouseDown);
	void updateApps(float deltaMs);
	void drawGraphicsObject(TabletGraphicsObject* obj, glm::mat4 transform);
	void updateScreen();
	void clear(glm::vec4 clearColor = {0,0,0,1});

	// The apps that run on the tablet
	std::vector<TabletApp*> apps;
	TabletApp* activeApp;
public:

	Tablet(glm::ivec2 resolution, float size, const vrlib::PositionalDevice& pointer, const vrlib::DigitalDevice& trigger, std::initializer_list<TabletApp*> apps);

	void update(float elapsedTime, vrlib::tien::Scene& scene) override;
	void postUpdate(vrlib::tien::Scene& scene) override;
	void drawForwardPass() override;

	template <typename A>
	A * getApp() {
		for (auto& c : apps)
		{
			A* r = dynamic_cast<A*>(c);
			if (r)
				return r;
		}
		return nullptr;
	}

	void setActiveApp(TabletApp* app);

	// Methods we need to implement to prevent being a virtual class
	void drawDeferredPass() override {};
	void drawShadowMap() override {};
	virtual void buildEditor(vrlib::tien::EditorBuilder* builder, bool folded) override {};
};