project "TruthEngine.Editor"
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
		IncludeDir["TruthEngine.Editor"],
		IncludeDir["TruthEngine.Core"],
		IncludeDir["spdlog"],
		IncludeDir["Dependencies"],
		IncludeDir["d3dx12"],
		IncludeDir["imgui"],
		IncludeDir["physx"],
	}

	links{
		"TruthEngine.Core",
		"ImGui",
	}
	
	postbuildcommands {
    "xcopy /y /d  %{wks.location}Dependencies\\Lib\\WinPixEventRuntime.dll $(TargetDir)",
	"xcopy /y /d  %{wks.location}Dependencies\\Lib\\dxcompiler.dll $(TargetDir)",
	"xcopy /y /d  %{wks.location}Dependencies\\Lib\\dxil.dll $(TargetDir)",
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
		
		postbuildcommands 
		{
			"xcopy /y /d  %{wks.location}Dependencies\\Lib\\Debug\\assimp-vc142-mtd.dll $(TargetDir)",
			"xcopy /y /d  %{wks.location}Dependencies\\Lib\\Debug\\assimp-vc142-mtd.pdb $(TargetDir)",
		}


	filter "configurations:Release"
		defines{
			"TE_RELEASE",
			"TE_NDEBUG"
		}
		runtime "Release"
		optimize "on"

		postbuildcommands 
		{
			"xcopy /y /d  %{wks.location}Dependencies\\Lib\\Release\\assimp-vc142-mt.dll $(TargetDir)"
		}

	filter "configurations:Dist"
		defines{
			"TE_DIST",
			"TE_NDEBUG"
		}
		runtime "Release"
		optimize "on"
		
		postbuildcommands 
		{
			"xcopy /y /d  %{wks.location}Dependencies\\Lib\\Release\\assimp-vc142-mt.dll $(TargetDir)"
		}

