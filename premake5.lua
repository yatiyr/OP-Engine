-- require "cmake"
include "OPDeps.lua"

workspace "OP"
    architecture "x86_64"
    startproject "OP_Editor"

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
group ""    
    


group "OPApps"
        include "Apps/RuntimeScripts"
group ""


group "ThirdParty"
    include "OP/external/GLFW"
    include "OP/external/Box2D"
    include "OP/external/Glad"
    include "OP/external/imgui"
    include "OP/external/yaml-cpp"
group ""
        