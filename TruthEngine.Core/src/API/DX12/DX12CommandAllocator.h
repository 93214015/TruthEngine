#pragma once

namespace TruthEngine::API::DirectX12 {

	class DX12CommandList;
	class DX12GraphicDevice;
	class CommandQueue;

	class DX12CommandAllocator {
	public:
		DX12CommandAllocator();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE commandListType, DX12GraphicDevice& gDevice);
		TE_RESULT Reset();
		inline ID3D12CommandAllocator* GetNativeObject() { return m_CommandAllocator.Get(); }
//		inline ID3D12CommandAllocator* operator->() { return m_CommandAllocator.Get(); }

		inline void Release() { m_CommandAllocator.Reset(); }

		bool IsRunning();
	private:
		COMPTR<ID3D12CommandAllocator> m_CommandAllocator;
		uint64_t m_FenceValue = 0;

		CommandQueue* m_RunningCommandQueue;

		//
		//Friend Classes
		//
		friend class DX12GraphicDevice;
		friend class CommandQueue;
		friend class DX12CommandList;
	};

}
