#include "pch.h"
#include "DirectX12Fence.h"
#include "DirectX12GraphicDevice.h"

namespace TruthEngine::API::DirectX12 {



	DirectX12Fence::DirectX12Fence()
	{
	}

	void DirectX12Fence::Initialize(const DirectX12GraphicDevice& device, D3D12_FENCE_FLAGS flags /*= D3D12_FENCE_FLAG_NONE*/)
	{
		device.GetDevice()->CreateFence(0, flags, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf()));
	}

	uint64_t DirectX12Fence::SetFence(ID3D12CommandQueue* cmdQueue)
	{
		cmdQueue->Signal(m_Fence.Get(), m_Value);
		return m_Value++;
	}


	HANDLE DirectX12Fence::SetFenceAndEvent(ID3D12CommandQueue* cmdQueue, LPCWSTR eventName)
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

	void DirectX12Fence::SetFenceAndEvent(ID3D12CommandQueue* cmdQueue, HANDLE eventHandle)
	{
		cmdQueue->Signal(m_Fence.Get(), m_Value);
		m_Fence->SetEventOnCompletion(m_Value, eventHandle);
		m_Value++;
	}


	HANDLE DirectX12Fence::SetEvent(uint64_t value, LPCWSTR eventName)
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

	void DirectX12Fence::SetEvent(uint64_t value, HANDLE eventHandle)
	{
		m_Fence->SetEventOnCompletion(value, eventHandle);
	}

	bool DirectX12Fence::Completed(uint64_t value)
	{
		if (m_Fence->GetCompletedValue() < value)
		{
			return false;
		}
		return true;
	}

}