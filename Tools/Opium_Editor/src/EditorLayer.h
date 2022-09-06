#pragma once

#include <Opium.h>
#include <GuiComponents/Scene/SceneUtilities/SceneHierarchyComponent.h>
#include <GuiComponents/Scene/SceneUtilities/ContentBrowserComponent.h>

// Temp
#include <GuiComponents/Scene/SceneUtilities/ViewportComponent.h>
#include <EditorResMonitor/EditorScriptMonitor.h>

#include <Renderer/EditorCamera.h>

#include <Scene/SceneRenderer.h>

#include <Geometry/Plane.h>
#include <Geometry/Cube.h>
#include <Geometry/Capsule.h>
#include <Geometry/Cylinder.h>
#include <Geometry/Icosphere.h>


namespace OP
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() { m_EditorScriptMonitor.Terminate(); m_EditorScriptMonitor.m_Thread.join(); }
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
		void SaveScene();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();


		// Temp
		void SetSelectedEntity();

		void SetShowPhysicalColliders(bool show) { m_ShowPhysicalColliders = show; }

	private:
		void RenderDockspace();
		Entity GetPrimaryCamera();

	private:
		// Components
		SceneHierarchyComponent m_SceneGraph;
		ContentBrowserComponent m_ContentBrowser;

		// Temp
		ViewportComponent m_ViewportComponent;


		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		Ref<Framebuffer> m_Framebuffer;
		Ref<Framebuffer> m_RenderFramebuffer;
		Ref<Framebuffer> m_FinalFramebuffer;
		Ref<Framebuffer> m_EntityIDFramebuffer;

		Ref<Shader> m_GridShader;
		Ref<Plane> m_Plane;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

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

		EditorScriptMonitor m_EditorScriptMonitor;



		bool m_ShowPhysicalColliders = true;
		Ref<Shader> m_CollisionShapeVisualizer;
		Ref<Cube> m_Cube;
		Ref<Icosphere> m_Sphere;
		Ref<Cylinder> m_Cylinder;
		Ref<Capsule> m_Capsule;

	private:
		static EditorLayer* s_Instance;


		friend class ViewportComponent;
	};
}