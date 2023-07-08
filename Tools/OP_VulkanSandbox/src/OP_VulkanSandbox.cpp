#include <Op.h>

#include <EntryPoint.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <Scene/Scene.h>


namespace OP
{

    Scene* s_ActiveScene;

    class OpVulkanSandbox : public Application
    {
    public:
        OpVulkanSandbox(AppCommandLineArguments args)
            : Application("Op Vulkan Sandbox", args)
        {

        }

        ~OpVulkanSandbox()
        {

        }
    };

    Application* CreateApplication(AppCommandLineArguments args)
    {
        return new OpVulkanSandbox(args);
    }
}
