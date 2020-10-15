#pragma once
#include "Fence.h"

namespace TruthEngine::API::DX12 {

	class CommandList;

	class DX12GraphicDevice;

	class CommandQueue {
		friend class DX12GraphicDevice;
		friend class DX12SwapChain;
	public:
		CommandQueue();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE type, DX12GraphicDevice& gDevice);
		TE_RESULT ExecuteCommandList(CommandList& cmdList) const;
		ID3D12CommandQueue* operator->() { return m_CommandQueue.Get(); }
		
	private:

	private:
		COMPTR<ID3D12CommandQueue> m_CommandQueue;
		
	};

}
