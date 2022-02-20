#include <Opium.h>

class ExampleLayer : public Opium::Layer
{
public:
    ExampleLayer() : Layer("Orneks") {}

    void OnUpdate() override
    {
        OP_APP_INFO("ExampleLayer::Update");
    }

    void OnEvent(Opium::Event& event) override
    {
        OP_APP_TRACE("{0}", event);
    }
};

class OpiumApp : public Opium::Application
{
public:
    OpiumApp()
    {
        PushLayer(new ExampleLayer());
        PushOverlay(new Opium::ImGuiLayer());
    }

    ~OpiumApp()
    {

    }
};

Opium::Application* Opium::CreateApplication()
{
    return new OpiumApp;
}