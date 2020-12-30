project "TruthEngine.Sandbox"
	kind "ConsoleApp"
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
		IncludeDir["TruthEngine.Sandbox"],
		IncludeDir["TruthEngine.Core"],
		IncludeDir["spdlog"],
		IncludeDir["Dependencies"],
		IncludeDir["d3dx12"],
		IncludeDir["imgui"]
	}

	libdirs 
	{ 
	"%{wks.location}/Dependencies/DirectXShaderCompiler/lib/x64",
	"%{wks.location}/Dependencies/assimp/lib" 
	}

	links{
		"TruthEngine.Core",
		"ImGui",
		"dxcompiler.dll"
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
		libdirs{
			"%{wks.location}/Dependencies/DirectXTK12/Lib/x64/Debug"
	    }
		defines{
			"TE_DEBUG"
		}
		runtime "Debug"
		symbols "on"


	filter "configurations:Release"
		libdirs{
			"%{wks.location}/Dependencies/DirectXTK12/Lib/x64/Release"
		}
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

