project "OP_VulkanSandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
	"src/**.hpp",
        "src/**.cpp",        
	"src/**.c"
    }

    includedirs
    {
        "%{wks.location}/OP/external/spdlog/include",
        "%{wks.location}/OP/src/Config",
        "%{wks.location}/OP/external",
        "%{IncludeDir.FontAwesome}",
        "%{wks.location}/OP/src",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.Bullet}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Mono}",
        "%{IncludeDir.glm}",
        "src"
    }

    links
    {
        "OP"
    }

    postbuildcommands
	{
	   '{COPY} "%{wks.location}/Tools/OP_VulkanSandbox/EditorResources" "%{cfg.targetDir}/EditorResources"',
	   '{COPY} "%{wks.location}/OP/external/mono/bin_/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"',
	   '{COPY} "%{wks.location}/OP/external/assimp/bin_/assimp-vc143-mt.dll" "%{cfg.targetDir}"',
           '{COPY} "%{wks.location}/Tools/OP_VulkanSandbox/assets" "%{cfg.targetdir}/assets"',
	   '{COPY} "%{wks.location}/Tools/OP_VulkanSandbox/mono" "%{cfg.targetDir}/mono"'
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