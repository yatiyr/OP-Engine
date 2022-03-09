#include <Precomp.h> 
#include <Opium/Application.h>

#include <Input/Input.h>

#include <Renderer/Renderer.h>

namespace Opium
{

	Application* Application::s_Instance = nullptr;


	Application::Application(const std::string& name)
	{
		OP_PROFILE_FUNCTION();

		OP_ENGINE_ASSERT(!s_Instance, "There is already an application ready!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create(WinProperties(name)));
		m_Window->SetEventCallback(OP_BIND_EVENT_FUNCTION(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		OP_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		OP_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		is_running = false;
	}

	void Application::OnEvent(Event& e)
	{
		OP_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(OP_BIND_EVENT_FUNCTION(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(OP_BIND_EVENT_FUNCTION(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		OP_PROFILE_FUNCTION();

		while (is_running)
		{

			OP_PROFILE_SCOPE("Run loop application");

			float time = (float)Application::GetWindow().GetTime();
			Timestep timestep = time - m_LastFrameTime;

			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					OP_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
			}

			m_ImGuiLayer->Begin();
			{
				OP_PROFILE_SCOPE("LayerStack OnImGuiRender");

				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();

		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		is_running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		OP_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}