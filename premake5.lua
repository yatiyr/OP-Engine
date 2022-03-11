-- require "cmake"

workspace "Opium"
    architecture "x64"
    startproject "Opium_Editor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Directories we which we need to include
IncludeDir = {}
IncludeDir["GLFW"] = "Opium/external/GLFW/include"
IncludeDir["Glad"] = "Opium/external/Glad/include"
IncludeDir["ImGui"] = "Opium/external/imgui"
IncludeDir["glm"] = "Opium/external/glm"
IncludeDir["stb_image"] = "Opium/external/stb_image"
IncludeDir["entt"] = "Opium/external/entt/include"
IncludeDir["yaml_cpp"] = "Opium/external/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "Opium/external/ImGuizmo"
IncludeDir["Mono"] = "Opium/external/mono/include"

LibraryDir = {}
LibraryDir["Mono"] = "%{wks.location}/Opium/external/mono/lib/Release/mono-2.0-sgen.lib"

--MonoBinDir = "%{wks.location}/Opium/external/mono/mono/bin"

group "ThirdParty"
include "Opium/external/GLFW"
include "Opium/external/Glad"
include "Opium/external/imgui"
include "Opium/external/yaml-cpp"
group ""

group "OpiumProjectCore"
project "Opium"
    location "Opium"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "Precomp.h"
    pchsource "Opium/src/Config/Precomp.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.hpp",
	    "%{prj.name}/src/**.cpp",
        "%{prj.name}/external/stb_image/**.h",
        "%{prj.name}/external/stb_image/**.cpp",
        "%{prj.name}/external/glm/glm/**.hpp",
        "%{prj.name}/external/glm/glm/**.inl",
        "%{prj.name}/external/entt/include/**.hpp",
        "%{prj.name}/external/ImGuizmo/*.h",
        "%{prj.name}/external/ImGuizmo/*.cpp"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
	    "YAML_CPP_STATIC_DEFINE"
    }

    includedirs
    {
        "%{prj.name}/src",
	    "%{prj.name}/src/Config",
        "%{prj.name}/external/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
	    "%{IncludeDir.Mono}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "yaml-cpp",
        "opengl32.lib",
	    "%{LibraryDir.Mono}"
    }

    filter "files:Opium/external/ImGuizmo/**.cpp"
    flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "OP_PLATFORM_WINDOWS",
            "OP_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "OP_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "OP_RELEASE"
        runtime "Release"
        optimize "speed"

    filter "configurations:Dist"
        defines "OP_DIST"
        runtime "Release"
        optimize "on"

project "Opium-Script"
	location "Opium-Script"
	kind "SharedLib"
	language "C#"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.cs"
	}
group ""


group "Opium-Tools"
project "Opium_Editor"
    location "Tools/Opium_Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Tools/%{prj.name}/src/**.h",
	    "Tools/%{prj.name}/src/**.hpp",
        "Tools/%{prj.name}/src/**.cpp",
	    "Tools/%{prj.name}/src/**.c"
    }

    includedirs
    {
        "Opium/external/spdlog/include",
        "Opium/src",
	    "Opium/src/Config",
        "Opium/external",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGuizmo}",
	    "Tools/%{prj.name}/src"
    }

    links
    {
        "Opium"
    }


    postbuildcommands
	{
           '{COPY} "%{wks.location}/Tools/Opium_Editor/assets" "%{cfg.targetdir}/assets"',
	       '{COPY} "%{wks.location}/Opium/external/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
	}


    filter "system:windows"
        systemversion "latest"

        defines
        {
            "OP_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "OP_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "OP_RELEASE"
        runtime "Release"
        optimize "speed"

    filter "configurations:Dist"
        defines "OP_DIST"
        runtime "Release"
        optimize "on"
group ""    
    


group "OpiumApps"
project "ExampleOpiumApp"
	location "Apps/ExampleOpiumApp"
	kind "SharedLib"
	language "C#"

	targetdir ("Tools/Opium_Editor/assets/scripts")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.cs"
	}

	links
	{
		"Opium-Script"
	}
--[[project "OpiumApp"
    location "Apps/OpiumApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Apps/%{prj.name}/src/**.h",
        "Apps/%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Opium/external/spdlog/include",
        "Opium/src",
	    "Opium/src/Config",
        "Opium/external",
        "%{IncludeDir.glm}",
	    "Apps/%{prj.name}/src"
    }

    links
    {
        "Opium"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "OP_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "OP_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "OP_RELEASE"
        runtime "Release"
        optimize "speed"

    filter "configurations:Dist"
        defines "OP_DIST"
        runtime "Release"
        optimize "on"
--]] 
group ""
        