-- require "cmake"
include "OpiumDeps.lua"

workspace "Opium"
    architecture "x86_64"
    startproject "Opium_Editor"

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

group "OpiumProjectCore"
    include "Opium"
    include "Opium-Script"
group ""


group "Opium-Tools"
    include "Tools/Opium_Editor"
group ""    
    


group "OpiumApps"
        include "Apps/ExampleOpiumApp"
group ""


group "ThirdParty"
    include "Opium/external/GLFW"
    include "Opium/external/Box2D"
    include "Opium/external/Glad"
    include "Opium/external/imgui"
    include "Opium/external/yaml-cpp"
group ""
        