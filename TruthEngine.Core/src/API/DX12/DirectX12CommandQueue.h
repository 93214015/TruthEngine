#pragma once
#include "DirectX12Fence.h"

namespace TruthEngine::API::DirectX12 {

	class DirectX12CommandList;

	class DirectX12GraphicDevice;

	class DirectX12CommandQueue {
		friend class DirectX12GraphicDevice;
		friend class DirectX12SwapChain;
	public:
		DirectX12CommandQueue();
		TE_RESULT Init(D3D12_COMMAND_LIST_TYPE type, DirectX12GraphicDevice* gDevice);
		virtual TE_RESULT ExecuteCommandList(DirectX12CommandList* cmdList) = 0;
		ID3D12CommandQueue* GetNativeObject() { return m_CommandQueue.Get(); }
		
	protected:

	protected:
		COMPTR<ID3D12CommandQueue> m_CommandQueue;
		
	};

	class DirectX12CommandQueue_Direct : public DirectX12CommandQueue
	{
	public:
		TE_RESULT Init(DirectX12GraphicDevice* gDevice);

		TE_RESULT ExecuteCommandList(DirectX12CommandList* cmdList) override;
	};

	class DirectX12CommandQueue_Copy : public DirectX12CommandQueue
	{
	public:
		TE_RESULT Init(DirectX12GraphicDevice* gDevice);

		TE_RESULT ExecuteCommandList(DirectX12CommandList* cmdList) override;
	};

}
