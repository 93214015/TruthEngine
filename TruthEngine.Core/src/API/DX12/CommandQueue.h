#pragma once
#include "Fence.h"

namespace TruthEngine::API::DirectX12 {

	class DX12CommandList;

	class DX12GraphicDevice;

	class CommandQueue {
		friend class DX12GraphicDevice;
		friend class DX12SwapChain;
	public:
		CommandQueue();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE type, DX12GraphicDevice* gDevice);
		virtual TE_RESULT ExecuteCommandList(DX12CommandList* cmdList) = 0;
		ID3D12CommandQueue* GetNativeObject() { return m_CommandQueue.Get(); }
		
	protected:

	protected:
		COMPTR<ID3D12CommandQueue> m_CommandQueue;
		
	};

	class CommandQueue_Direct : public CommandQueue
	{
	public:
		TE_RESULT Init(DX12GraphicDevice* gDevice);

		TE_RESULT ExecuteCommandList(DX12CommandList* cmdList) override;
	};

	class CommandQueue_Copy : public CommandQueue
	{
	public:
		TE_RESULT Init(DX12GraphicDevice* gDevice);

		TE_RESULT ExecuteCommandList(DX12CommandList* cmdList) override;
	};

}
