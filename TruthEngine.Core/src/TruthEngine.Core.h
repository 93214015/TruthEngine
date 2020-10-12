#pragma once

#ifdef TE_BUILD_DLL

	#define TRUTHENGINE_API _declspec(dllexport)

#else

	#define TRUTHENGINE_API _declspec(dllimport)

#endif


#ifdef TE_ENABLE_ASSERTS

	#define TE_ASSERT_CORE(x, ...) { if(!(x)) { TE_LOG_CORE_ERROR("TE_Error: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TE_ASSERT_APP(x, ...) { if(!(x)) { TE_LOG_APP_ERROR("TE_Error: {0}", __VA_ARGS__); __debugbreak(); } }

#else

	#define TE_ASSERT_CORE(x, ...)
	#define TE_ASSERT_APP(x, ...)

#endif


#define BIT(x) (1 << x)


enum TE_RESULT : int {
	TE_FAIL = -1,
	TE_SUCCESSFUL = 0
};

#define TE_SUCCEEDED(te_result) (((TE_RESULT)te_result) >= 0)
#define TE_FAILED(te_result) (((TE_RESULT)te_result) < 0)