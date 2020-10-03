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
IncludeDir["Dependencies"] = "%{wks.location}/Dependencies"
IncludeDir["spdlog"] = "%{wks.location}/Dependencies/spdlog/include"

include "TruthEngineCore"
include "TruthEngineSandboxApp"
include "TruthEngineDX12"
include "TruthEngineWin32App"