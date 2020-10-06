

workspace "TruthEngine"
	architecture "x64"
	startproject "TruthEngineSandboxApp"

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
IncludeDir["TruthEngineCore"] = "%{wks.location}/TruthEngineCore/src"
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


	