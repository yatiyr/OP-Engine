-- require "cmake"
include "OPDeps.lua"

workspace "OP"
    architecture "x86_64"
    startproject "OP_VulkanSandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "OPProjectCore"
    include "OP"
    include "OP-Script"
group ""


group "OP-Tools"
    include "Tools/OP_Editor"
    include "Tools/OP_VulkanSandbox"
group ""    
    


group "OPApps"
        include "Apps/RuntimeScripts"
group ""


group "ThirdParty"
    include "OP/external/glfw"
    include "OP/external/box2D"
    include "OP/external/glad"
    include "OP/external/imgui"
    include "OP/external/yaml-cpp"
group ""
        