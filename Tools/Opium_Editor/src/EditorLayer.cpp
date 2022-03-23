#include <EditorLayer.h>
#include <imgui/imgui.h>
#include <imgui/implot.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Scene/SceneSerializer.h>

#include <Utils/PlatformUtils.h>

#include <ImGuizmo/ImGuizmo.h>

#include <Math/Math.h>


// #include <IconsMaterialDesign.h>

#include <Gui/Font/Font.h>
#include <GuiComponents/GuiBlocks/GuiBlockGenerator.h>

namespace Opium
{

	extern ImFont* ImGuiIconFontBg;
	extern ImFont* ImGuiIconFontMd;
	extern ImFont* ImGuiIconFontText;

	EditorLayer* EditorLayer::s_Instance = nullptr;
	extern const std::filesystem::path  g_AssetPath;

	Scene* s_ActiveScene;

	EditorLayer* EditorLayer::CreateEditor()
	{
		s_Instance = new EditorLayer();
		return s_Instance;
	}

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{

	}

	void EditorLayer::OnAttach()
	{

		m_IconPlay = Texture2D::Create("EditorResources/Icons/playbutton_icon.png");
		m_IconStop = Texture2D::Create("EditorResources/Icons/stopbutton_icon.png");


		// Set up framebuffer to be sent to viewport
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);
		/////////////////////////////////////////////////////////

		// Temp
		m_ViewportComponent.SetFramebuffer(m_Framebuffer);
		m_ActiveScene = CreateRef<Scene>();

		auto commandLineArguments = Application::Get().GetCommandLineArguments();
		if (commandLineArguments.Count > 1)
		{
			auto sceneFilePath = commandLineArguments[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.DeserializeText(sceneFilePath);
		}

		s_ActiveScene = m_ActiveScene.get();

		m_EditorCamera = EditorCamera(30.0f, 1280.0f / 720.0f, 0.1, 1000.0f);

		// For testing C# integration
		auto scriptedEntity = m_ActiveScene->CreateEntity("ScriptedEntity");
		scriptedEntity.AddComponent<ScriptComponent>("Example.Script");
		bool x = scriptedEntity.HasComponent<ScriptComponent>();


		m_SceneGraph.SetContext(m_ActiveScene);

	}

	void EditorLayer::RenderDockspace()
	{

		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_AutoHideTabBar;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("##Opium_EditorDockspace", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;
	}

	Entity EditorLayer::GetPrimaryCamera()
	{
		// Render 2D
		return m_ActiveScene->GetPrimaryCameraEntity();
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{

		m_ViewportComponent.ResizeFramebuffer();

		// Render
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();


		// Clear our entity ID attachment to -1
		m_Framebuffer->ClearAttachment(1, -1);


		switch (m_SceneState)
		{
			case SceneState::Edit:
			{

				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}

		m_ViewportComponent.SetHoveredEntity();
		

		m_Framebuffer->Unbind();

		
	}

	void EditorLayer::OnImGuiRender()
	{

		RenderDockspace();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10,10 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{ 1000, 200 });
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				// ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }
				/*if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; } */
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		m_SceneGraph.OnImGuiRender();
		m_ContentBrowser.OnImGuiRender();


		ImGui::SetNextWindowSize(ImVec2(100, 32), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 50, 50 });
		ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);



		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d:", stats.DrawCalls);
		ImGui::Text("QuadCount: %d:", stats.QuadCount);
		ImGui::Text("Vertices: %d:", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d:", stats.GetTotalIndexCount());
		

		// bool showDemoPlot = true;
		// ImPlot::GetStyle().AntiAliasedLines = true;
		// ImPlot::ShowDemoWindow(&showDemoPlot);
		ImGui::End();
		ImGui::PopStyleVar();

		bool x = true;
		ImGui::ShowDemoWindow(&x);

		m_ViewportComponent.OnImGuiRender();

		UI_Toolbar();

		ImGui::End();
		
		{
			ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));
			ImGui::Begin("OP_Test Properties Layout", nullptr, ImGuiWindowFlags_NoTitleBar);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);

			// search button and menu
			{
				ImGui::BeginChild("OP_Test_Top", ImVec2(0, 32), true);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
					ImGui::PushFont(ImGuiIconFontMd);
					ImGui::Button(OP_ICON);
					ImGui::PopFont();
					ImGui::PopStyleVar();
				ImGui::EndChild();
			}
			
			
			// icons
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.152f, 0.152f, 0.152f, 1.0f });
				ImGui::BeginChild("left", ImVec2(36, 0), true);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 6, 6 });
					ImGui::PushFont(ImGuiIconFontMd);
					ImGui::Button(OP_ICON_TRANSFORM);
					ImGui::Button(OP_ICON_PHYSICS);
					ImGui::Button(OP_ICON_LIGHT);
					ImGui::PopFont();
					ImGui::PopStyleVar();
				ImGui::EndChild();
				ImGui::PopStyleColor();
			}

			ImGui::SameLine();

			ImGui::BeginChild("OP_TestChild_1");
				ImGui::Text("Hello from child 1");
			ImGui::EndChild();


			ImGui::PopStyleVar();
			ImGui::End();
			ImGui::PopStyleVar();
		}

	}

	void EditorLayer::UI_Toolbar()
	{

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0 , 0));
		auto& colors = ImGui::GetStyle().Colors;
		auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize);


		float size = ImGui::GetWindowHeight() - 4.0f;

		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1,1), 0))
		{
			// Add Pause, simulate 
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}

		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();

	}

	void EditorLayer::SetSelectedEntity()
	{
		if(m_HoveredEntity)
			m_SceneGraph.SetSelectedEntity(m_HoveredEntity);
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(OP_BIND_EVENT_FUNCTION(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(OP_BIND_EVENT_FUNCTION(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		m_ViewportComponent.OnMouseButtonPressed(e);
		
		return false;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool controlPressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shiftPressed = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (controlPressed)
				{
					NewScene();
				}

				break;
			}

			case Key::O:
			{
				if (controlPressed)
				{
					OpenScene();
				}

				break;
			}

			case Key::S:
			{
				if (controlPressed && shiftPressed)
				{
					SaveSceneAs();
				}

				break;
			}

			// Gizmo stuff
			case Key::Q:
				m_GizmoType = -1;
				break;
			case Key::W:
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}


		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneGraph.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filePath = FileDialogs::OpenFile("Opium Scene file (*.opium)\0*.opium\0");
		if (!filePath.empty())
		{
			OpenScene(filePath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneGraph.SetContext(m_ActiveScene);

		SceneSerializer serializer(m_ActiveScene);
		serializer.DeserializeText(path.string());
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filePath = FileDialogs::SaveFile("Opium Scene file (*.opium)\0*.opium\0");
		if (!filePath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.SerializeText(filePath);
		}
	}

	void EditorLayer::OnScenePlay()
	{
		m_ActiveScene->OnRuntimeStart();
		m_SceneState = SceneState::Play;
	}

	void EditorLayer::OnSceneStop()
	{
		m_ActiveScene->OnRuntimeStop();
		m_SceneState = SceneState::Edit;
	}

}