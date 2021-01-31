project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	location "imgui"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


	files{
		"imgui/*.h",
		"imgui/*.cpp",
		"imgui/backends/imgui_impl_dx11.h",
		"imgui/backends/imgui_impl_dx11.cpp",
		"imgui/backends/imgui_impl_dx12.h",
		"imgui/backends/imgui_impl_dx12.cpp",
		"imgui/backends/imgui_impl_win32.h",
		"imgui/backends/imgui_impl_win32.cpp",
		"ImGuizmo/ImGuizmo.h",
		"ImGuizmo/ImGuizmo.cpp"
	}


	defines{
		
	}

	links{

	}

	--[[buildoptions {
		"/fp:fast"
	}--]]

	filter "system:windows"
		systemversion "latest"


	filter "configurations:Debug"
		defines{
			"_DEBUG",
			"DEBUG"
		}
		runtime "Debug"
		symbols "on"


	filter "configurations:Release"
		defines{
			"_NDEBUG",
			"NDEBUG"
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines{
			"NDEBUG",
			"_NDEBUG"
		}
		runtime "Release"
		optimize "on"
