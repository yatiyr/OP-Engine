#pragma once

#include <Opium.h>
#include <GuiComponents/SceneHierarchyComponent.h>
#include <GuiComponents/ContentBrowserComponent.h>

#include <Renderer/EditorCamera.h>

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
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		OrthographicCameraController m_CameraController;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;

		Entity m_SquareEntity;

		Entity m_CameraEntity;
		Entity m_CameraEntity2;

		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;
		EditorCamera m_EditorCamera;

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
		ContentBrowserComponent m_ContentBrowser;

		int m_GizmoType = -1;

		Entity m_ScriptedTestEntity;

	};
}