VK_SDK = os.getenv("VULKAN_SDK")

-- Directories we which we need to include
IncludeDir = {}
IncludeDir["GLFW"]        = "%{wks.location}/Opium/external/GLFW/include"
IncludeDir["Glad"]        = "%{wks.location}/Opium/external/Glad/include"
IncludeDir["ImGui"]       = "%{wks.location}/Opium/external/imgui"
IncludeDir["glm"]         = "%{wks.location}/Opium/external/glm"
IncludeDir["stb_image"]   = "%{wks.location}/Opium/external/stb_image"
IncludeDir["entt"]        = "%{wks.location}/Opium/external/entt/include"
IncludeDir["yaml_cpp"]    = "%{wks.location}/Opium/external/yaml-cpp/include"
IncludeDir["ImGuizmo"]    = "%{wks.location}/Opium/external/ImGuizmo"
IncludeDir["Mono"]        = "%{wks.location}/Opium/external/mono/include"
IncludeDir["shaderc"]     = "%{wks.location}/Opium/external/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Opium/external/SPIRV-Cross"
IncludeDir["VK_SDK"]      = "%{VK_SDK}/Include"
 
LibraryDir = {}
LibraryDir["Mono"]   = "%{wks.location}/Opium/external/mono/lib/Release"
LibraryDir["VK_SDK"] = "%{VK_SDK}/Lib"

Library = {}
Library["Mono"]     = "%{LibraryDir.Mono}/mono-2.0-sgen.lib"
Library["VK"]       = "%{LibraryDir.VK_SDK}/vulkan-1.lib"
Library["VK_UTILS"] = "%{LibraryDir.VK_SDK}/VkLayer_utils.lib"

Library["ShaderC_DBG"]          = "%{LibraryDir.VK_SDK}/shaderc_sharedd.lib"
Library["SPIRV_CROSS_DBG"]      = "%{LibraryDir.VK_SDK}/spirv-cross-cored.lib"
Library["SPIRV_CROSS_GLSL_DBG"] = "%{LibraryDir.VK_SDK}/spirv-cross-glsld.lib"
Library["SPIRV_TOOLS_DBG"]      = "%{LibraryDir.VK_SDK}/SPIRV-Toolsd.lib"

Library["ShaderC_REL"]          = "%{LibraryDir.VK_SDK}/shaderc_shared.lib"
Library["SPIRV_CROSS_REL"]      = "%{LibraryDir.VK_SDK}/spirv-cross-core.lib"
Library["SPIRV_CROSS_GLSL_REL"] = "%{LibraryDir.VK_SDK}/spirv-cross-glsl.lib"
Library["SPIRV_TOOLS_DBG"]      = "%{LibraryDir.VK_SDK}/SPIRV-Tools.lib"
