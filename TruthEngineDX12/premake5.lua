project "TruthEngineDX12"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"


	files{
		"src/**.h",
		"src/**.cpp"
	}


	defines{

	}

	includedirs{
		"src",
		IncludeDir["spdlog"],
		IncludeDir["Dependencies"]
	}

	links{
		"TruthEngineCore"
	}

	floatingpoint "Fast"

	--[[buildoptions {
		"/fp:fast"
	}--]]

	filter "system:windows"
		systemversion "latest"

		defines{
			"TE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines{
			"TE_DEBUG"
		}
		runtime "Debug"
		symbols "on"


	filter "configurations:Release"
		defines{
			"TE_RELEASE",
			"TE_NDEBUG"
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines{
			"TE_DIST",
			"TE_NDEBUG"
		}
		runtime "Release"
		optimize "on"

