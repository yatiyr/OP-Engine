#include <Op.h>

#include <EntryPoint.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <EditorLayer.h>


namespace OP
{
    class OpEditor : public Application
    {
    public:
        OpEditor(AppCommandLineArguments args)
            : Application("Op Editor", args)
        {
            EditorLayer* editorLayer = EditorLayer::CreateEditor();
            PushLayer(editorLayer);
        }

        ~OpEditor()
        {

        }
    };

    Application* CreateApplication(AppCommandLineArguments args)
    {
        return new OpEditor(args);
    }
}
