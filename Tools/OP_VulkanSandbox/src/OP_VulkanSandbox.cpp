#include <Op.h>

#include <EntryPoint.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <Scene/Scene.h>


namespace OP
{

    Scene* s_ActiveScene;

    class OpEditor : public Application
    {
    public:
        OpEditor(AppCommandLineArguments args)
            : Application("Op Vulkan Sandbox", args)
        {

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
