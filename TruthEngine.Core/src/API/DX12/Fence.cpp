#include "pch.h"
#include "Fence.h"
#include "DX12GraphicDevice.h"

namespace TruthEngine::API::DirectX12 {



	Fence::Fence()
	{
	}

	void Fence::Initialize(const DX12GraphicDevice& device, D3D12_FENCE_FLAGS flags /*= D3D12_FENCE_FLAG_NONE*/)
	{
		device.GetDevice()->CreateFence(0, flags, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf()));
	}

	uint64_t Fence::SetFence(ID3D12CommandQueue* cmdQueue)
	{
		cmdQueue->Signal(m_Fence.Get(), m_Value);
		return m_Value++;
	}


	HANDLE Fence::SetFenceAndEvent(ID3D12CommandQueue* cmdQueue, LPCWSTR eventName)
	{
		cmdQueue->Signal(m_Fence.Get(), m_Value);
		auto value = m_Value;
		m_Value++;
		if (m_Fence->GetCompletedValue() < value)
		{
			HANDLE h = CreateEvent(nullptr, FALSE, FALSE, eventName);
			m_Fence->SetEventOnCompletion(value, h);
			return h;
		}
		else {
			return nullptr;
		}

	}


	HANDLE Fence::SetEvent(uint64_t value, LPCWSTR eventName)
	{
		if (m_Fence->GetCompletedValue() < value)
		{
			HANDLE h = CreateEvent(nullptr, FALSE, FALSE, eventName);
			m_Fence->SetEventOnCompletion(value, h);
			return h;
		}
		else {
			return nullptr;
		}
	}

	int Fence::SetEvent(uint64_t value, HANDLE eventHandle)
	{
		if (m_Fence->GetCompletedValue() < value)
		{
			m_Fence->SetEventOnCompletion(value, eventHandle);
			return 1;
		}
		else {
			return 0;
		}
	}

	bool Fence::Completed(uint64_t value)
	{
		if (m_Fence->GetCompletedValue() < value)
		{
			return false;
		}
		return true;
	}

}