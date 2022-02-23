#pragma once

#include <Window/Window.h>
#include <Opium/Core.h>
#include <Opium/Logging.h>
#include <Opium/LayerStack.h>
#include <EventSystem/Event.h>
#include <EventSystem/AppEvent.h>

#include <Gui/ImGuiLayer.h>



namespace Opium
{
	class OPIUM_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool is_running = true;

		LayerStack m_LayerStack;

		static Application* s_Instance;
	};

	// Client will define this
	Application* CreateApplication();
}