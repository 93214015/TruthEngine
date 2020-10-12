#pragma once

namespace TruthEngine::API::DX12 {

	class GDeviceDX12;

	class Fence {

	public:
		Fence();
		void Initialize(const GDeviceDX12& device, D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE);

		uint64_t SetFence(ID3D12CommandQueue* cmdQueue);
		HANDLE SetFenceAndEvent(ID3D12CommandQueue* cmdQueue, LPCWSTR eventName);
		HANDLE SetEvent(uint64_t value, LPCWSTR eventName);
		int SetEvent(uint64_t value, HANDLE eventHandle);
		bool Check(uint64_t value);
	protected:

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		uint64_t m_Value = 1;

	};

}
