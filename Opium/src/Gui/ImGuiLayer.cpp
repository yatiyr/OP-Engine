#include <Precomp.h>
#include <Gui/ImGuiLayer.h>

#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <Opium/Application.h>
#include <GLFW/glfw3.h>

#include <ImGuizmo.h>

#include <Gui/Font/Font.h>

namespace OP
{
	extern ImFont* ImGuiIconFontBg = nullptr;
	extern ImFont* ImGuiIconFontMd = nullptr;
	extern ImFont* ImGuiIconFontText = nullptr;

	std::unordered_map<std::string, void*> ImGuiFontTable{};

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		OP_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		// Add implot too
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		// io.ConfigViewportsNoTaskBarIcon = true;

		LoadFonts("Inter");
		// LoadFonts("Ubuntu");
		// LoadFonts("OpenSans");
		LoadIconFonts();

		io.FontDefault = (ImFont*)ImGuiFontTable["Inter-Light-14"];
		

		// Setup Dear ImGui styles
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();
		SetGlobalStyles();


		// Setup Platform/Renderer backends
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);


		ImGui_ImplOpenGL3_Init("#version 410");
	
	}

	void ImGuiLayer::OnDetach()
	{
		OP_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		OP_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

	}

	void ImGuiLayer::End()
	{
		OP_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void* ImGuiLayer::GetFontPtr(const std::string& fontName)
	{
		return ImGuiFontTable[fontName];
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Text Colors
		colors[ImGuiCol_Text]         = ImVec4{ 0.9f, 0.9f, 0.9f, 1.0f };
		colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

		// Window Colors
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_ChildBg]  = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };

		// Border Colors
		colors[ImGuiCol_Border]       = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
		colors[ImGuiCol_BorderShadow] = ImVec4{ 0.1f, 0.1f, 0.1f, 0.6f };

		// Frame Colors
		colors[ImGuiCol_FrameBg]        = ImVec4{ 0.110f, 0.110f, 0.110f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.130f, 0.130f, 0.130f, 1.0f };
		colors[ImGuiCol_FrameBgActive]  = ImVec4{ 0.160f, 0.160f, 0.160f, 1.0f };

		// Title Colors
		colors[ImGuiCol_TitleBg]          = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_TitleBgActive]    = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };

		// Menu bar Color
		colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.110f, 0.110f, 0.110f, 1.0f };

		// Scrollbar  Colors
		colors[ImGuiCol_ScrollbarBg]          = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_ScrollbarGrab]        = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.260f, 0.260f, 0.260f, 0.8f };
		colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4{ 0.290f, 0.290f, 0.290f, 0.9f };

		// Checkmark
		colors[ImGuiCol_CheckMark] = ImVec4{ 0.932f, 0.631f, 0.058f, 0.8f };

		// Slider Colors
		colors[ImGuiCol_SliderGrab]       = ImVec4{ 0.400f, 0.400f, 0.400f, 0.7f };
		colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.600f, 0.600f, 0.600f, 0.7f };

		// Button Colors
		colors[ImGuiCol_Button]        = ImVec4{ 0.210f, 0.210f, 0.210f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.240f, 0.240f, 0.240f, 1.0f };
		colors[ImGuiCol_ButtonActive]  = ImVec4{ 0.280f, 0.280f, 0.280f, 1.0f };

		// TreeNode, Selectable, Menu Item, Collapsing Header Colors
		colors[ImGuiCol_Header] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };

		// Separator Colors TODO: Sort This Out!!!!
		colors[ImGuiCol_Separator] = ImVec4{ 0.1, 0.1, 0.1, 1.0f };
		colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
		colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };


		// Resize Grip
		colors[ImGuiCol_ResizeGrip]        = ImVec4{ 0.200f, 0.200f, 0.200f, 0.8f };
		colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.250f, 0.250f, 0.250f, 0.8f };
		colors[ImGuiCol_ResizeGripActive]  = ImVec4{ 0.270f, 0.270f, 0.270f, 0.8f };

		// Tab Colors
		colors[ImGuiCol_Tab] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.240f, 0.240f, 0.240f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.220f, 0.220f, 0.220f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.190f, 0.190f, 0.190f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.200f, 0.200f, 0.200f, 1.0f };

		// TODO: Docking, PlotLines, Histogram, Table, DragDrop Target, Nav, Model colors
		// need to be implemented!

	}

	void ImGuiLayer::SetGlobalStyles()
	{
		ImGuiStyle& style = ImGui::GetStyle();


		// TODO: Global styles will be implemented!
		style.WindowBorderSize = 1.0f;
		style.TabRounding = 0.0f;
		style.AntiAliasedLines = true;
		style.FrameBorderSize = 0.0f;
		style.ChildBorderSize = 0.0f;
		style.PopupBorderSize = 1.0f;

		style.ScrollbarRounding = false;
		style.WindowTitleAlign = ImVec2{ 0.5f, 0.5f };

		style.TabBorderSize = 0.0f;
		style.WindowRounding = 1.0f;
		style.FrameRounding = 2.0f;

		// style.WindowPadding = ImVec2{ 0.0f, 0.0f };

		style.AntiAliasedLinesUseTex = true;
		style.AntiAliasedFill = true;

		style.ScaleAllSizes(Application::Get().GetDpiScale() * 1.1f);

		
	}

	void ImGuiLayer::LoadFonts(const std::string& fontName)
	{

		OP_ENGINE_WARN("{0}", Application::Get().GetDpiScale());

		float dpiScale = Application::Get().GetDpiScale();

		ImGuiIO& io = ImGui::GetIO();
		// io.Fonts->AddFontDefault();

		std::string fontStyles[4] = { "Light", "Regular", "Medium", "Bold" };
		int fontSizes[8] = { 6, 8, 10, 12, 14, 16, 18, 20 };

		for (int i = 0; i < 1; i++)
		{
			for (int j = 0; j < 1; j++)
			{
				ImFontConfig fontCfg;
				fontCfg.OversampleH = 8;
				fontCfg.OversampleV = 2;
				fontCfg.RasterizerMultiply = 1.2f;

				std::string path = std::string("assets/fonts/") + fontName + "/" + fontName + "-" + fontStyles[1] + ".ttf";
				std::string finalName = fontName + "-" + fontStyles[1] + "-" + std::to_string(fontSizes[4]);
				ImGuiFontTable[finalName] = (void*)io.Fonts->AddFontFromFileTTF(path.c_str(), fontSizes[4] * dpiScale * 1.1f, &fontCfg);
			}
		}
	}

	void ImGuiLayer::LoadIconFonts()
	{
		float dpiScale = Application::Get().GetDpiScale();

		ImFontConfig fontCfg;
		fontCfg.OversampleH = 1;
		fontCfg.OversampleV = 1;
		fontCfg.RasterizerMultiply = 1.2f;
		// fontCfg.MergeMode = true;
		// fontCfg.GlyphMinAdvanceX = 28.0f * dpiScale;
		// fontCfg.GlyphOffset.y = 10.0;
		// fontCfg.GlyphMinAdvanceX = 28.0f;
		// fontCfg.PixelSnapH = true;

		ImGuiIO& io = ImGui::GetIO();
		static const ImWchar icons_ranges[] = { ICON_MIN_OP, ICON_MAX_OP, 0 };
		ImGuiIconFontBg = io.Fonts->AddFontFromFileTTF("assets/fonts/Webfonts/icomoon.ttf", 32.0f * dpiScale, &fontCfg, icons_ranges);

		ImGuiIconFontMd = io.Fonts->AddFontFromFileTTF("assets/fonts/Webfonts/icomoon.ttf", 24.0f * dpiScale, &fontCfg, icons_ranges);

		ImGuiIconFontText = io.Fonts->AddFontFromFileTTF("assets/fonts/Webfonts/icomoon.ttf", 14.0f * dpiScale, &fontCfg, icons_ranges);
	}

}

