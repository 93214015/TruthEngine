#include "pch.h"
#include "Core/Profiler/GPUEvents.h"

#include "WinPixEventRuntime/pix3.h"

#include "API/DX12/DirectX12CommandList.h"

#ifdef TE_PLATFORM_WINDOWS

namespace TruthEngine
{
	namespace Profiler
	{

		void GPUEvent::BeginEvent(RendererCommand& _RendererCommand, const char* _EventName)
		{
			BeginEvent(_RendererCommand.m_CurrentCommandList, _EventName);
		}

		void GPUEvent::BeginEvent(CommandList& _CommandList, const char* _EventName)
		{
			BeginEvent(&_CommandList, _EventName);
		}

		void GPUEvent::BeginEvent(CommandList* _CommandList, const char* _EventName)
		{
			TE_ASSERT_CORE(_CommandList, "GPUBeginEvent: CommandList is nullptr");
			auto _DX12CommandList = static_cast<API::DirectX12::DirectX12CommandList*>(_CommandList);
			PIXBeginEvent(_DX12CommandList->GetNativeObject(), 0, _EventName);
		}

		void GPUEvent::EndEvent(RendererCommand& _RendererCommand)
		{
			EndEvent(_RendererCommand.m_CurrentCommandList);
		}

		void GPUEvent::EndEvent(CommandList& _CommandList)
		{
			EndEvent(&_CommandList);
		}

		void GPUEvent::EndEvent(CommandList* _CommandList)
		{
			TE_ASSERT_CORE(_CommandList, "GPUEndEvent: CommandList is nullptr");
			auto _DX12CommandList = static_cast<API::DirectX12::DirectX12CommandList*>(_CommandList);
			PIXEndEvent(_DX12CommandList->GetNativeObject());
		}

	}
}

#endif