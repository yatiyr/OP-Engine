project "OP"
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
	"external/tinyexr/**.h",
	"external/tinyexr/**.cpp",
	"external/tinyexr/**.c",
        "external/glm/glm/**.hpp",
        "external/glm/glm/**.inl",
        "external/entt/include/**.hpp",
        "external/ImGuizmo/*.h",
        "external/ImGuizmo/*.cpp",
	"external/bullet/*.hpp",
	"external/bullet/*.h"
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
	"%{IncludeDir.tinyexr}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
	"%{IncludeDir.Mono}",
        "%{IncludeDir.VK_SDK}",
	"%{IncludeDir.FontAwesome}",
	"%{IncludeDir.SPIRV_Cross}",
	"%{IncludeDir.Assimp}",
	"%{IncludeDir.Bullet}"
    }

    links
    {
        "Box2D",
        "GLFW",
        "Glad",
        "ImGui",
        "yaml-cpp",
        "opengl32.lib",
	"%{Library.Mono}",
	"%{Library.Assimp}",
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
			"%{Library.SPIRV_CROSS_GLSL_DBG}",
	"%{LibraryDir.BulletDbg}/Bullet3Collision_Debug.lib",
	"%{LibraryDir.BulletDbg}/Bullet3Common_Debug.lib",
	"%{LibraryDir.BulletDbg}/Bullet3Dynamics_Debug.lib",
	"%{LibraryDir.BulletDbg}/Bullet3Geometry_Debug.lib",
	"%{LibraryDir.BulletDbg}/Bullet3OpenCL_clew_Debug.lib",
	"%{LibraryDir.BulletDbg}/BulletCollision_Debug.lib",
	"%{LibraryDir.BulletDbg}/BulletDynamics_Debug.lib",
	"%{LibraryDir.BulletDbg}/BulletInverseDynamics_Debug.lib",
	"%{LibraryDir.BulletDbg}/BulletInverseDynamicsUtils_Debug.lib",
	"%{LibraryDir.BulletDbg}/BulletSoftBody_Debug.lib",
	"%{LibraryDir.BulletDbg}/ConvexDecomposition_Debug.lib",
	"%{LibraryDir.BulletDbg}/LinearMath_Debug.lib"
        }

    filter "configurations:Release"
        defines "OP_RELEASE"
        runtime "Release"
        optimize "speed"

        links
        {
            "%{Library.ShaderC_REL}",
			"%{Library.SPIRV_CROSS_REL}",
			"%{Library.SPIRV_CROSS_GLSL_REL}",
	"%{LibraryDir.BulletRel}/Bullet3Collision.lib",
	"%{LibraryDir.BulletRel}/Bullet3Common.lib",
	"%{LibraryDir.BulletRel}/Bullet3Dynamics.lib",
	"%{LibraryDir.BulletRel}/Bullet3Geometry.lib",
	"%{LibraryDir.BulletRel}/Bullet3OpenCL_clew.lib",
	"%{LibraryDir.BulletRel}/BulletCollision.lib",
	"%{LibraryDir.BulletRel}/BulletDynamics.lib",
	"%{LibraryDir.BulletRel}/BulletInverseDynamics.lib",
	"%{LibraryDir.BulletRel}/BulletInverseDynamicsUtils.lib",
	"%{LibraryDir.BulletRel}/BulletSoftBody.lib",
	"%{LibraryDir.BulletRel}/ConvexDecomposition.lib",
	"%{LibraryDir.BulletRel}/LinearMath.lib"
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