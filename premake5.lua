--[[ include "./Dependencies/premake/premake_customization/*--]]
--[[include "./Dependencies/premake/premake_customization/fp_fast.lua"--]]

workspace "TruthEngine"
	architecture "x64"
	startproject "TruthEngineSandboxApp"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

	--[[solution_items{
		".editorconfig"
	}--]]
	

	flags{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["TruthEngineCore"] = "%{wks.location}/TruthEngineCore/src"
IncludeDir["TruthEngineDX12"] = "%{wks.location}/TruthEngineDX12/src"
IncludeDir["TruthEngineWin32"] = "%{wks.location}/TruthEngineWin32/src"
IncludeDir["TruthEngineSandboxApp"] = "%{wks.location}/TruthEngineSandboxApp/src"
IncludeDir["Dependencies"] = "%{wks.location}/Dependencies"
IncludeDir["spdlog"] = "%{wks.location}/Dependencies/spdlog/include"

filter ("configurations.Debug")
		defines{
			"TE_ENABLE_ASSERTS",
		}
filter{}


include "TruthEngineCore"
include "TruthEngineSandboxApp"
include "TruthEngineDX12"
include "TruthEngineWin32"


	