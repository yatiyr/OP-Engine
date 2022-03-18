project "Opium_Editor"
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
        "%{wks.location}/Opium/external/spdlog/include",
        "%{wks.location}/Opium/src",
	    "%{wks.location}/Opium/src/Config",
        "%{wks.location}/Opium/external",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
	"%{IncludeDir.FontAwesome}",
        "%{IncludeDir.ImGuizmo}",
	    "src"
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