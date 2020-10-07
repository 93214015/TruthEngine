#include "pch.h"
#include "Fence.h"

namespace TruthEngine::API::DX12 {



	Fence::Fence() = default;

	void Fence::Initialize(ID3D12Device* device, D3D12_FENCE_FLAGS flags /*= D3D12_FENCE_FLAG_NONE*/)
	{
		device->CreateFence(0, flags, IID_PPV_ARGS(mFence.ReleaseAndGetAddressOf()));
	}

	uint64_t Fence::SetFence(ID3D12CommandQueue* cmdQueue)
	{
		cmdQueue->Signal(mFence.Get(), mValue);
		auto value = mValue;
		mValue++;
		return value;
	}

	HANDLE Fence::SetEvent(uint64_t value, LPCWSTR eventName)
	{
		if (mFence->GetCompletedValue() < value)
		{
			HANDLE h = CreateEvent(nullptr, FALSE, FALSE, eventName);
			mFence->SetEventOnCompletion(value, h);
			return h;
		}
		else {
			return nullptr;
		}
	}

	int Fence::SetEvent(uint64_t value, HANDLE eventHandle)
	{
		if (mFence->GetCompletedValue() < value)
		{
			mFence->SetEventOnCompletion(value, eventHandle);
			return 1;
		}
		else {
			return 0;
		}
	}

	bool Fence::Check(uint64_t value)
	{
		if (mFence->GetCompletedValue() < value)
		{
			return false;
		}
		return true;
	}

}