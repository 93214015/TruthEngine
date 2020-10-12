

workspace "TruthEngine"
	architecture "x64"
	startproject "TruthEngine.Sandbox"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}
	

	flags{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["TruthEngine.Core"] 		= 	"%{wks.location}/TruthEngine.Core/src"
IncludeDir["TruthEngine.Sandbox"] 	= 	"%{wks.location}/TruthEngine.Sandbox/src"
IncludeDir["Dependencies"] 			= 	"%{wks.location}/Dependencies"
IncludeDir["spdlog"] 				= 	"%{wks.location}/Dependencies/spdlog/include"
IncludeDir["imgui"] 				= 	"%{wks.location}/Dependencies/imgui"
IncludeDir["d3dx12"] 				= 	"%{wks.location}/Dependencies/DirectXGraphicsSamples/Libraries/D3DX12"

filter ("configurations.Debug")
		defines{
			"TE_ENABLE_ASSERTS",
		}
filter{}


include "TruthEngine.Core"
include "TruthEngine.Sandbox"
include "Dependencies/imgui"


	