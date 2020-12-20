#pragma once

namespace TruthEngine::API::DirectX12 {

	class DirectX12CommandList;
	class DirectX12GraphicDevice;
	class DirectX12CommandQueue;

	class DirectX12CommandAllocator {
	public:
		DirectX12CommandAllocator();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE commandListType, DirectX12GraphicDevice& gDevice);
		TE_RESULT Reset();
		inline ID3D12CommandAllocator* GetNativeObject() { return m_CommandAllocator.Get(); }
//		inline ID3D12CommandAllocator* operator->() { return m_CommandAllocator.Get(); }

		inline void Release() { m_CommandAllocator.Reset(); }

		bool IsRunning();
	private:
		COMPTR<ID3D12CommandAllocator> m_CommandAllocator;
		uint64_t m_FenceValue = 0;

		DirectX12CommandQueue* m_RunningCommandQueue;

		//
		//Friend Classes
		//
		friend class DirectX12GraphicDevice;
		friend class DirectX12CommandQueue;
		friend class DirectX12CommandQueue_Direct;
		friend class DirectX12CommandQueue_Copy;
		friend class DirectX12CommandList;
	};

}
