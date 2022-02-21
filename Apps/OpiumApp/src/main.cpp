#include <Opium.h>
#include <imgui.h>

class ExampleLayer : public Opium::Layer
{
public:
    ExampleLayer() : Layer("Orneks") {}

    void OnUpdate() override
    {
        //OP_APP_INFO("ExampleLayer::Update");

        if (Opium::Input::IsKeyPressed(OP_KEY_E))
            OP_APP_INFO("E key is pressed (poll)!")

    }

    void OnEvent(Opium::Event& event) override
    {
        //OP_APP_TRACE("{0}", event);

        if (event.GetEventType() == Opium::EventType::KeyPressed)
        {
            Opium::KeyPressedEvent& e = (Opium::KeyPressedEvent&)event;
            if (e.GetKeyCode() == OP_KEY_E)
                OP_APP_TRACE("E key is pressed (event)!");
        }
    }

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Test");
        ImGui::Text("Hello World");
        ImGui::End();
    }
};

class OpiumApp : public Opium::Application
{
public:
    OpiumApp()
    {
        PushLayer(new ExampleLayer());
    }

    ~OpiumApp()
    {

    }
};

Opium::Application* Opium::CreateApplication()
{
    return new OpiumApp;
}