#include <Precomp.h>
#include "SceneRenderer.h"

#include <Renderer/VertexArray.h>
#include <Renderer/Shader.h>
#include <Renderer/UniformBuffer.h>
#include <Renderer/RenderCommand.h>

#include <Renderer/Texture.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Temp

#include <glad/glad.h>

#include <Geometry/Icosphere.h>

namespace OP
{

	struct SceneRendererData
	{
		// Shaders
		Ref<Shader> mainShader;
		Ref<Shader> depthShader;
		Ref<Shader> depthDebugShader;

		// Ref<Texture2D> woodTexture;
		Ref<Texture2D> WhiteTexture;

		Ref<Icosphere> icosphere;
		// Ref<Icosphere> icosphere2;

		glm::vec3 lightPos{-2.0f, 4.0f, -1.0f};


		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec3 ViewPos;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;



		glm::vec2 ViewportSize{0.0f, 0.0f};


		// Framebuffer
		Ref<Framebuffer> depthFramebuffer;
		Ref<Framebuffer> finalFramebuffer;

	};


	static SceneRendererData s_SceneRendererData;

	void SceneRenderer::Init(float width, float height, Ref<Framebuffer> fB)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		s_SceneRendererData.icosphere = Icosphere::Create(1.0f, 2, true);

		s_SceneRendererData.mainShader = Shader::Create("assets/shaders/Pbr/Main.glsl");
		s_SceneRendererData.depthShader = Shader::Create("assets/shaders/Pbr/Depth.glsl");
		s_SceneRendererData.depthDebugShader = Shader::Create("assets/shaders/Pbr/DepthDebug.glsl");

		s_SceneRendererData.CameraUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::CameraData), 0);
		
		s_SceneRendererData.ViewportSize.x = width;
		s_SceneRendererData.ViewportSize.y = height;

		// Create white texture
		s_SceneRendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_SceneRendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// Arrange Framebuffers

		// Depth framebuffer
		FramebufferSpecification depthFBSpec;
		depthFBSpec.Attachments = { FramebufferTextureFormat::DEPTH16 };
		depthFBSpec.Width = s_SceneRendererData.ViewportSize.x;
		depthFBSpec.Height = s_SceneRendererData.ViewportSize.y;
		s_SceneRendererData.depthFramebuffer = Framebuffer::Create(depthFBSpec);

		// Final Framebuffer
		
		/*FramebufferSpecification finalFBSpec;
		finalFBSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		finalFBSpec.Width = s_SceneRendererData.ViewportSize.x;
		finalFBSpec.Height = s_SceneRendererData.ViewportSize.y;
		s_SceneRendererData.finalFramebuffer = Framebuffer::Create(finalFBSpec); */

		s_SceneRendererData.finalFramebuffer = fB;


		// Configure Shaders
		s_SceneRendererData.mainShader->Bind();
		s_SceneRendererData.mainShader->SetInt("u_DiffuseTexture", 0);
		s_SceneRendererData.mainShader->SetInt("u_ShadowMap", 0);

		s_SceneRendererData.depthDebugShader->Bind();
		s_SceneRendererData.depthDebugShader->SetInt("u_DepthMap", 0);


	}

	void SceneRenderer::ResizeViewport(float width, float height)
	{
		if (s_SceneRendererData.ViewportSize.x != width || s_SceneRendererData.ViewportSize.y != height)
		{
			// Update viewport size
			s_SceneRendererData.ViewportSize.x = width;
			s_SceneRendererData.ViewportSize.y = height;

			// Resize necessary framebuffers
			s_SceneRendererData.depthFramebuffer->Resize(width, height);
			s_SceneRendererData.finalFramebuffer->Resize(width, height);
		}
	}

	void SceneRenderer::Render(EditorCamera& camera)
	{
		s_SceneRendererData.CameraBuffer.ViewProjection = camera.GetProjection();
		s_SceneRendererData.CameraBuffer.ViewPos = camera.GetPosition();
		s_SceneRendererData.CameraUniformBuffer->SetData(&s_SceneRendererData.CameraBuffer, sizeof(SceneRendererData::CameraData));

		// Render depth of scene to texture (from light's perspective)
		// -----------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 0.2f, far_plane = 7.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(s_SceneRendererData.lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		// render scene from light's point of view
		s_SceneRendererData.depthShader->Bind();
		s_SceneRendererData.depthShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, s_SceneRendererData.ViewportSize.x, s_SceneRendererData.ViewportSize.y);

		s_SceneRendererData.depthFramebuffer->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			s_SceneRendererData.WhiteTexture->Bind();
			glCullFace(GL_FRONT);
			glm::mat4 model = glm::mat4(1.0f);
			s_SceneRendererData.depthShader->SetMat4("u_Model", model);
			s_SceneRendererData.icosphere->Draw();
			model = glm::translate(model, glm::vec3(2.0f, 1.0f, 1.0f));
			s_SceneRendererData.depthShader->SetMat4("u_Model", model);
			s_SceneRendererData.icosphere->Draw();
			glCullFace(GL_BACK);
		s_SceneRendererData.depthFramebuffer->Unbind();


		// render the scene normally

		s_SceneRendererData.finalFramebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_SceneRendererData.mainShader->Bind();
		s_SceneRendererData.mainShader->SetFloat3("u_LightPos", s_SceneRendererData.lightPos);
		s_SceneRendererData.mainShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		s_SceneRendererData.WhiteTexture->Bind();
		glActiveTexture(GL_TEXTURE1);
		// simdilik kapa glBindTexture(GL_TEXTURE_2D, s_SceneRendererData.depthFramebuffer->GetColorAttachmentRendererID(0));
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
		s_SceneRendererData.mainShader->SetMat4("u_Models", model);
		s_SceneRendererData.icosphere->Draw();
		
		//model = glm::translate(model, glm::vec3(2.0f, 1.0f, 1.0f));
		//s_SceneRendererData.mainShader->SetMat4("u_Model", model);
		//s_SceneRendererData.icosphere->Draw();

	}

	SceneRendererStats SceneRenderer::GetStats()
	{
		return SceneRendererStats();
	}

}