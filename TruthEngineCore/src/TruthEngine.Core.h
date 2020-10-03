#pragma once

#ifdef TE_BUILD_DLL
#define TRUTHENGINE_API _declspec(dllexport)
#else
#define TRUTHENGINE_API _declspec(dllimport)
#endif

