#pragma once

namespace TruthEngine::API::DX12 {

	class Fence {

	public:
		Fence();

		void Initialize(ID3D12Device* device, D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE);
		uint64_t SetFence(ID3D12CommandQueue* cmdQueue);
		HANDLE SetEvent(uint64_t value, LPCWSTR eventName);
		int SetEvent(uint64_t value, HANDLE eventHandle);
		bool Check(uint64_t value);
	protected:
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		uint64_t mValue = 1;

	};

}
