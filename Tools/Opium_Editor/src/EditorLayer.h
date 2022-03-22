#pragma once

#include <Opium.h>
#include <GuiComponents/Scene/SceneUtilities/SceneHierarchyComponent.h>
#include <GuiComponents/Scene/SceneUtilities/ContentBrowserComponent.h>

// Temp
#include <GuiComponents/Scene/SceneUtilities/ViewportComponent.h>

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


		static EditorLayer* CreateEditor();
		static EditorLayer* GetEditor() { return s_Instance; }

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneStop();

		// UI Panels
		void UI_Toolbar();


		// Temp
		void SetSelectedEntity();

	private:
		void RenderDockspace();

	private:
		// Components
		SceneHierarchyComponent m_SceneGraph;
		ContentBrowserComponent m_ContentBrowser;

		// Temp
		ViewportComponent m_ViewportComponent;



		OrthographicCameraController m_CameraController;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;

		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;


		Ref<Texture2D> m_IconPlay;
		Ref<Texture2D> m_IconStop;

		int m_GizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;

	private:
		static EditorLayer* s_Instance;


		friend class ViewportComponent;
	};
}