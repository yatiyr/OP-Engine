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

namespace OP
{

	struct SceneRendererData
	{
		// Shaders
		Ref<Shader> colorShader;
		Ref<Shader> depthShader;
		Ref<Shader> depthDebugShader;

		Ref<Texture2D> woodTexture;


		glm::vec3 lightPos{-2.0f, 4.0f, -1.0f};

	};


	static SceneRendererData s_SceneRendererData;

	void SceneRenderer::Init()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void SceneRenderer::Render()
	{
	}

	SceneRendererStats SceneRenderer::GetStats()
	{
		return SceneRendererStats();
	}

}