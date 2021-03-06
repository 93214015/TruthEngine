#pragma once

#define AI_MAKE_EMBEDDED_TEXNAME "***"

#include "lib.h"

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <tchar.h>
#include <vector>
#include <cassert>
#include <queue>
#include <functional>
#include <sstream>
#include <array> //std::array
#include <chrono>
#include <future> //std::async & std::future
#include <unordered_map> //std::unordered_Map
#include <map>	   //std::map
#include <variant> //std::varinat
#include <optional> //std::optional
#include <string_view> //std::string_view

#include "DirectXMath/Inc/DirectXMath.h"
#include "DirectXMath/Inc/DirectXColors.h"
#include "DirectXMath/Inc/DirectXPackedVector.h"
#include "DirectXMath/Inc/DirectXCollision.h"



//////////////////////////////////////////////////////////////////////////
//// ImGui Headers
//////////////////////////////////////////////////////////////////////////
#include "imgui/imgui.h"
#include "imgui_internal.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "ImGuizmo/ImGuizmo.h"



//////////////////////////////////////////////////////////////////////////
///Assimp Library Header Files
//////////////////////////////////////////////////////////////////////////
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/config.h"


//////////////////////////////////////////////////////////////////////////
///Assimp Library Header Files
//////////////////////////////////////////////////////////////////////////
#include "entt/entt.hpp"


//////////////////////////////////////////////////////////////////////////
///Physx Library Header Files
//////////////////////////////////////////////////////////////////////////
#include "PxConfig.h"
#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"


//////////////////////////////////////////////////////////////////////////
//// TE Headers
//////////////////////////////////////////////////////////////////////////
#include "Core/Core.h"
#include "Core/ThreadPool.h"
#include "Core/Log.h"
#include "Core/Timer.h"


//////////////////////////////////////////////////////////////////////////
////Windows Platform Header Files
//////////////////////////////////////////////////////////////////////////

#ifdef TE_PLATFORM_WINDOWS

// // Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <ppl.h>
#include <wincodec.h>
#include <wincodecsdk.h>

template<class T>
using COMPTR = Microsoft::WRL::ComPtr<T>;


#endif

//////////////////////////////////////////////////////////////////////////////
/// Graphic APIs Header
/// //////////////////////////////////////////////////////////////////////////

//
// IDXGI
//
#ifdef TE_DEBUG
	#include <dxgidebug.h>
#endif

#include <dxgi1_6.h>

//
// HLSL Compiler
//
#include <d3dcompiler.h>

#ifdef TE_API_DX12

//
//DirectX12
//
#include <d3d12.h>
#include "d3dx12.h"

#endif
