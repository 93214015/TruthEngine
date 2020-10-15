#include "pch.h"
#include "CommandAllocator.h"
#include "DX12GraphicDevice.h"

namespace TruthEngine::API::DX12 {


	TE_RESULT CommandAllocator::Reset()
	{
		auto& fence = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence();

		HANDLE e = CreateEventA(nullptr, false, false, NULL);

		if (fence.SetEvent(m_FenceValue, e) == 1)
		{
			WaitForSingleObject(e, INFINITE);
		}

		return static_cast<TE_RESULT>(m_CommandAllocator->Reset());

	}

	TE_RESULT CommandAllocator::Init(D3D12_COMMAND_LIST_TYPE commandListType, DX12GraphicDevice& gDevice)
	{
		return gDevice.CreateCommandAllocator(m_CommandAllocator, commandListType);
	}

	CommandAllocator::CommandAllocator() = default;
}