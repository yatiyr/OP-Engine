#pragma once

#include <Opium.h>
#include <GuiComponents/SceneHierarchyComponent.h>

namespace Opium
{
	class EditorLayer : public Opium::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() {}
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;
	private:
		OrthographicCameraController m_CameraController;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;

		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_CameraEntity2;

		bool m_PrimaryCamera = true;

		// Temporary
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_Spritesheet;
		Ref<SubTexture2D> m_TextureStairs;
		Ref<SubTexture2D> m_TextureBush;
		Ref<SubTexture2D> m_TextureTree;
		Ref<SubTexture2D> m_TextureBarrel;

		// Components
		SceneHierarchyComponent m_SceneGraph;

	};
}