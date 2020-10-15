#pragma once

namespace TruthEngine::API::DX12 {

	class CommandList;

	class CommandAllocator {
		friend class DX12GraphicDevice;
		friend class CommandQueue;
		friend class CommandList;
	public:
		CommandAllocator();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE commandListType, DX12GraphicDevice& gDevice);
		TE_RESULT Reset();
		ID3D12CommandAllocator* Get() { return m_CommandAllocator.Get(); }
		ID3D12CommandAllocator* operator->() { return m_CommandAllocator.Get(); }

		inline void Release() { m_CommandAllocator.Reset(); }
	private:
		COMPTR<ID3D12CommandAllocator> m_CommandAllocator;
		uint64_t m_FenceValue = 0;
	};

}
