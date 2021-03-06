#pragma once

namespace TruthEngine::API::DirectX12 {

	class DirectX12GraphicDevice;

	class DirectX12Fence {

	public:
		DirectX12Fence();
		void Initialize(const DirectX12GraphicDevice& device, D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE);

		uint64_t SetFence(ID3D12CommandQueue* cmdQueue);
		HANDLE SetFenceAndEvent(ID3D12CommandQueue* cmdQueue, LPCWSTR eventName);
		void SetFenceAndEvent(ID3D12CommandQueue* cmdQueue, HANDLE eventHandle);
		HANDLE SetEvent(uint64_t value, LPCWSTR eventName);
		void SetEvent(uint64_t value, HANDLE eventHandle);
		bool Completed(uint64_t value);
	protected:

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		uint64_t m_Value = 1;

	};

}
