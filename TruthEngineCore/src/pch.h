#pragma once

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


#include "DirectXMath/Inc/DirectXMath.h"
#include "DirectXMath/Inc/DirectXColors.h"
#include "DirectXMath/Inc/DirectXPackedVector.h"
#include "DirectXMath/Inc/DirectXCollision.h"

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
#include <wrl/client.h>

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

//
//DirectX12
//
#include <d3d12.h>
#include <d3dcompiler.h>
