#pragma once

#include <Window/Window.h>
#include <Op/Core.h>
#include <Op/Logging.h>
#include <Op/LayerStack.h>
#include <EventSystem/Event.h>
#include <EventSystem/AppEvent.h>

#include <Op/Timestep.h>

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

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }

		AppCommandLineArguments GetCommandLineArguments() const { return m_CommandLineArguments; }

		float GetDpiScale() { return m_DpiScale; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		AppCommandLineArguments m_CommandLineArguments;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool is_running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		float m_DpiScale = 0.0f;
	private:
		static Application* s_Instance;
	};

	// Client will define this
	Application* CreateApplication(AppCommandLineArguments args);
}