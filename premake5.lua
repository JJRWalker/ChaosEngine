workspace "ChaosEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Distribution"
    }

    startproject  "Game"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    IncludeDir = {}
    IncludeDir["spdlog"] = "%{prj.name}/vendor/spdlog/include"
    IncludeDir["GLFW"] = "%{prj.name}/vendor/GLFW/include"
    IncludeDir["GLM"] = "%{prj.name}/vendor/GLM/glm"
    IncludeDir["Vulkan"] = "%{prj.name}/vendor/Vulkan/include"

    include "ChaosEngine/vendor/GLFW"
    include "ChaosEngine/vendor/Vulkan"

project "ChaosEngine"
    location "ChaosEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/".. outputdir .. "/%{prj.name}")
    objdir ("int/".. outputdir .. "/%{prj.name}")
    
    pchheader "chaospch.h"
    pchsource "ChaosEngine/src/chaospch.cpp"

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/Vulkan/include/**.hpp",
        "%{prj.name}/vendor/Vulkan/include/**.h"
    }
    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE",
        "CHAOS_PLATFORM_WINDOWS"
    }
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.Vulkan}"
    }

    links
    {
        "GLFW",
        "ChaosEngine/vendor/Vulkan/lib/vulkan-1.lib",
        "Vulkan",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "CHAOS_PLATFORM_WINDOWS"
        }
        
        filter "configurations:Debug"
            defines "CHAOS_DEBUG"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines "CHAOS_RELEASE"
            runtime "Release"
            optimize "on"

        filter "configurations:Distribution"
            defines "CHAOS_DISTRIBUTION"
            runtime "Release"
            optimize "on"

project "Game"
    location "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/".. outputdir .. "/%{prj.name}")
    objdir ("int/".. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"

    }

    includedirs
    {
        "ChaosEngine/src",
        "ChaosEngine/vendor",
        "ChaosEngine/vendor/spdlog/include"
    }

    links
    {
        "ChaosEngine"
    }

    filter "system:windows"
        systemversion "latest"
        
        defines
        {
            "CHAOS_PLATFORM_WINDOWS"
        }
        
        filter "configurations:Debug"
            defines "CHAOS_DEBUG"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines "CHAOS_RELEASE"
            runtime "Release"
            optimize "on"

        filter "configurations:Distribution"
            defines "CHAOS_DISTRIBUTION"
            runtime "Release"
            optimize "on"