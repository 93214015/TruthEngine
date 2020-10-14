#pragma once

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <cassert>
#include <queue>
#include <functional>
#include <sstream>
#include <array> //std::array
#include <chrono>


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

template<class T>
using COMPTR = Microsoft::WRL::ComPtr<T>;

#endif

#ifdef TE_API_DX12

//////////////////////////////////////////////////////////////////////////
//// DirectX12 API Header Files
//////////////////////////////////////////////////////////////////////////
#include <d3d12.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

#endif
