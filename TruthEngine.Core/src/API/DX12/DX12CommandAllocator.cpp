#include "pch.h"
#include "DX12CommandAllocator.h"
#include "DX12GraphicDevice.h"

namespace TruthEngine::API::DirectX12 {


	TE_RESULT DX12CommandAllocator::Reset()
	{

		if (IsRunning())
		{
			auto& fence = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence();

			HANDLE e = CreateEventA(nullptr, false, false, NULL);

			if (fence.SetEvent(m_FenceValue, e) == 1)
			{
				WaitForSingleObject(e, INFINITE);
			}
		}

		return static_cast<TE_RESULT>(m_CommandAllocator->Reset());

	}

	TE_RESULT DX12CommandAllocator::Init(D3D12_COMMAND_LIST_TYPE commandListType, DX12GraphicDevice& gDevice)
	{
		return gDevice.CreateCommandAllocator(m_CommandAllocator, commandListType);
	}

	bool DX12CommandAllocator::IsRunning()
	{
		return !(TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().Completed(m_FenceValue));
	}

	DX12CommandAllocator::DX12CommandAllocator() = default;
}