#pragma once
#include "Fence.h"

namespace TruthEngine::API::DX12 {

	class CommandList;

	class GDeviceDX12;

	class CommandQueue {
		friend class GDeviceDX12;
		friend class SwapChainDX12;
	public:
		CommandQueue();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE type, GDeviceDX12& gDevice);
		TE_RESULT ExecuteCommandList(CommandList& cmdList) const;
		ID3D12CommandQueue* operator->() { return m_CommandQueue.Get(); }
		
	private:

	private:
		COMPTR<ID3D12CommandQueue> m_CommandQueue;
		
	};

}
