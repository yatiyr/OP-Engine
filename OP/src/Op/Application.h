#pragma once

// Op Core Headers
#include <Op/Core.h>
#include <Op/Logging.h>
#include <Op/Timestep.h>
#include <Op/LayerStack.h>

// Event Headers
#include <EventSystem/Event.h>
#include <EventSystem/AppEvent.h>

// Window Header
#include <Window/Window.h>

// Gui header
#include <Gui/ImGuiLayer.h>


namespace OP
{
	struct AppCommandLineArguments
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			OP_ENGINE_ASSERT(index < Count);
			return Args[index];
		}
	};

	class Application
	{
	public:
		Application(const std::string& name = "Noname Application", AppCommandLineArguments args = AppCommandLineArguments());

		virtual ~Application();

		void Run();
		void Close();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		float GetDpiScale() { return m_DpiScale; }

		AppCommandLineArguments GetCommandLineArguments() const { return m_CommandLineArguments; }

		static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		bool is_running = true;
		bool m_Minimized = false;

		float m_DpiScale = 0.0f;
		float m_LastFrameTime = 0.0f;

		AppCommandLineArguments m_CommandLineArguments;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		std::unique_ptr<Window> m_Window;
	private:
		static Application* s_Instance;
	};

	// Client will define this
	Application* CreateApplication(AppCommandLineArguments args);
}