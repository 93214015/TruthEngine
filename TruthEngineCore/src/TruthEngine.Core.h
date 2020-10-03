#pragma once

#ifdef TE_PLATFORM_WINDOWS

	#ifdef TE_BUILD_DLL
		#define TRUTHENGINE_API _declspec(dllexport)
	#else
		#define TRUTHENGINE_API _declspec(dllimport)
	#endif

#else

	#error TruthEngine just support windows at the moment

#endif
