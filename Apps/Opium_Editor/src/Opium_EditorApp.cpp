#include <Opium.h>

#include <Opium/EntryPoint.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <EditorLayer.h>


namespace Opium
{
    class OpiumEditor : public Application
    {
    public:
        OpiumEditor()
            : Application("Opium Editor")
        {
            PushLayer(new EditorLayer());
        }

        ~OpiumEditor()
        {

        }
    };

    Application* CreateApplication()
    {
        return new OpiumEditor;
    }
}
