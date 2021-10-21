#pragma once
#include "Core/Renderer/RendererCommand.h"

namespace TruthEngine
{

	namespace Profiler
	{
		class GPUEvent
		{
		public:
			static void BeginEvent(RendererCommand& _RendererCommand, const char* _EventName);
			static void BeginEvent(CommandList& _RendererCommand, const char* _EventName);
			static void BeginEvent(CommandList* _RendererCommand, const char* _EventName);
			static void EndEvent(RendererCommand& _RendererCommand);
			static void EndEvent(CommandList& _RendererCommand);
			static void EndEvent(CommandList* _RendererCommand);
		};
	}

}

#if (defined(_DEBUG) || defined(_PROFILE))

#define TE_GPUBEGINEVENT(_Context, _EventName) Profiler::GPUEvent::BeginEvent(_Context, _EventName)
#define TE_GPUENDEVENT(_Context) Profiler::GPUEvent::EndEvent(_Context)

#else

#define TE_GPUBEGINEVENT(_Context, _EventName)
#define TE_GPUENDEVENT(_Context)

#endif

