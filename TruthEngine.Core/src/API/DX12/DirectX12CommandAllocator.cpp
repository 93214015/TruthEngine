#include "pch.h"
#include "DirectX12CommandAllocator.h"
#include "DirectX12GraphicDevice.h"

namespace TruthEngine::API::DirectX12 {


	TE_RESULT DirectX12CommandAllocator::Reset()
	{

		if (IsRunning())
		{
			auto& fence = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence();

			HANDLE e = CreateEventA(nullptr, false, false, NULL);

			fence.SetEvent(m_FenceValue, e);
			WaitForSingleObject(e, INFINITE);
		}

		return static_cast<TE_RESULT>(m_CommandAllocator->Reset());

	}

	TE_RESULT DirectX12CommandAllocator::Init(D3D12_COMMAND_LIST_TYPE commandListType, DirectX12GraphicDevice& gDevice)
	{
		event = CreateEventA(NULL, false, true, "");

		return gDevice.CreateCommandAllocator(m_CommandAllocator, commandListType);

	}

	bool DirectX12CommandAllocator::IsRunning()
	{
		return !(TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().Completed(m_FenceValue));
	}

	void DirectX12CommandAllocator::WaitToFinish()
	{
		auto dx12Fence = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence();
		dx12Fence.SetEvent(m_FenceValue, event);
		WaitForSingleObject(event, INFINITE);
	}

	DirectX12CommandAllocator::DirectX12CommandAllocator() = default;
}