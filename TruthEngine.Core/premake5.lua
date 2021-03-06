project "TruthEngine.Core"
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
		"TE_API_DX12"
	}

	includedirs{
		IncludeDir["TruthEngine.Core"],
		IncludeDir["spdlog"],
		IncludeDir["Dependencies"],
		IncludeDir["imgui"],
		IncludeDir["d3dx12"],
		IncludeDir["physx"],
		IncludeDir["boostIncPath"]
	}

	links{
		"ImGui",
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

