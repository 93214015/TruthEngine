#pragma once

namespace TruthEngine::API::DX12 {

	class CommandList;

	class CommandAllocator {
		friend class GDeviceDX12;
		friend class CommandQueue;
		friend class CommandList;
	public:
		CommandAllocator();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE commandListType, GDeviceDX12& gDevice);
		TE_RESULT Reset();
		ID3D12CommandAllocator* Get() { return m_CommandAllocator.Get(); }
		ID3D12CommandAllocator* operator->() { return m_CommandAllocator.Get(); }

		inline void Release() { m_CommandAllocator.Reset(); }
	private:
		COMPTR<ID3D12CommandAllocator> m_CommandAllocator;
		uint64_t m_FenceValue = 0;
	};

}
