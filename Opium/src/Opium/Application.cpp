#include <Precomp.h> 
#include <Opium/Application.h>
#include <Opium/ResourceManager.h>
#include <Renderer/RenderCommand.h>

#include <Input/Input.h>

#include <Renderer/Renderer.h>

#include <ScriptManager/ScriptManager.h>


#include <PhysicsManager/PhysicsManager.h>

namespace OP
{

	Application* Application::s_Instance = nullptr;


	Application::Application(const std::string& name, AppCommandLineArguments args)
		: m_CommandLineArguments(args)
	{

		OP_ENGINE_ASSERT(!s_Instance, "There is already an application ready!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create(WinProperties(name)));
		m_Window->SetEventCallback(OP_BIND_EVENT_FUNCTION(Application::OnEvent));

		m_DpiScale = m_Window->GetDpiScale();

		RenderCommand::Enable(MODE::DITHER);
		RenderCommand::Enable(MODE::TEXTURE_CUBE_MAP_SEAMLESS);

		ResourceManager::Init(std::filesystem::current_path());
		PhysicsManager::Init();
		Renderer::Init();
		ScriptManager::InitializeManager("assets/scripts/bin/RuntimeScripts.dll");

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		is_running = false;
	}

	void Application::OnEvent(Event& e)
	{
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

		while (is_running)
		{

			float time = (float)Application::GetWindow().GetTime();
			Timestep timestep = time - m_LastFrameTime;

			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
			}

			m_ImGuiLayer->Begin();
			{

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