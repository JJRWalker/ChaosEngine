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
        "%{prj.name}/src/**.cpp"
    }
    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }
    includedirs
    {
        "%{IncludeDir.spdlog}"
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
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "ChaosEngine/src",
        "ChaosEngine/vendor",
        "ChaosEngine/vendor/spdlog/include",
        "%{IncludeDir.spdlog}"
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