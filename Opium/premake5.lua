project "Opium"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "Precomp.h"
    pchsource "src/Config/Precomp.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp",
	    "src/**.cpp",
        "external/stb_image/**.h",
        "external/stb_image/**.cpp",
        "external/glm/glm/**.hpp",
        "external/glm/glm/**.inl",
        "external/entt/include/**.hpp",
        "external/ImGuizmo/*.h",
        "external/ImGuizmo/*.cpp"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
	    "YAML_CPP_STATIC_DEFINE",
        "GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "src",
	    "src/Config",
        "external/spdlog/include",
        "%{IncludeDir.Box2D}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
	    "%{IncludeDir.Mono}",
        "%{IncludeDir.VK_SDK}",
	"%{IncludeDir.FontAwesome}"
    }

    links
    {
        "Box2D",
        "GLFW",
        "Glad",
        "ImGui",
        "yaml-cpp",
        "opengl32.lib",
	    "%{Library.Mono}"
    }

    filter "files:external/ImGuizmo/**.cpp"
    flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "OP_PLATFORM_WINDOWS",
            "OP_BUILD_DLL",
        }

    filter "configurations:Debug"
        defines "OP_DEBUG"
        runtime "Debug"
        symbols "on"

        links
        {
            "%{Library.ShaderC_DBG}",
			"%{Library.SPIRV_CROSS_DBG}",
			"%{Library.SPIRV_CROSS_GLSL_DBG}"
        }

    filter "configurations:Release"
        defines "OP_RELEASE"
        runtime "Release"
        optimize "speed"

        links
        {
            "%{Library.ShaderC_REL}",
			"%{Library.SPIRV_CROSS_REL}",
			"%{Library.SPIRV_CROSS_GLSL_REL}"
        }        

    filter "configurations:Dist"
        defines "OP_DIST"
        runtime "Release"
        optimize "on"

        links
        {
            "%{Library.ShaderC_REL}",
			"%{Library.SPIRV_CROSS_REL}",
			"%{Library.SPIRV_CROSS_GLSL_REL}"
        }           