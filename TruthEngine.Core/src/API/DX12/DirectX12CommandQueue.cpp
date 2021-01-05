#include "pch.h"
#include "DirectX12CommandQueue.h"
#include "DirectX12CommandList.h"
#include "DirectX12GraphicDevice.h"

namespace TruthEngine::API::DirectX12 {



	DirectX12CommandQueue::DirectX12CommandQueue() = default;

	/*TE_RESULT DirectX12CommandQueue::ExecuteCommandList(DirectX12CommandList* cmdList)
	{

		auto c = cmdList->GetNativeObject();
		c->Close();

		ID3D12CommandList* l[1] = { c };
		m_CommandQueue->ExecuteCommandLists(1, l);

		cmdList->m_CommandAllocator.m_FenceValue = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().SetFence(m_CommandQueue.Get());
		cmdList->m_CommandAllocator.m_RunningCommandQueue = this;

		return TE_SUCCESSFUL;

	}*/

	TE_RESULT DirectX12CommandQueue::Init(D3D12_COMMAND_LIST_TYPE type, DirectX12GraphicDevice* gDevice)
	{
		return gDevice->CreateCommandQueue(m_CommandQueue, type);
	}

	TE_RESULT DirectX12CommandQueue_Direct::Init(DirectX12GraphicDevice* gDevice)
	{
		return DirectX12CommandQueue::Init(D3D12_COMMAND_LIST_TYPE_DIRECT, gDevice);
	}

	TE_RESULT DirectX12CommandQueue_Direct::ExecuteCommandList(DirectX12CommandList* cmdList)
	{
		auto c = cmdList->m_D3D12CommandList.Get();
		c->Close();

		ID3D12CommandList* l[1] = { c };
		m_CommandQueue->ExecuteCommandLists(1, l);

		cmdList->m_CommandAllocator.m_FenceValue = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().SetFence(m_CommandQueue.Get());
		cmdList->m_CommandAllocator.m_RunningCommandQueue = this;

		return TE_SUCCESSFUL;
	}

	TE_RESULT DirectX12CommandQueue_Copy::Init(DirectX12GraphicDevice* gDevice)
	{
		return DirectX12CommandQueue::Init(D3D12_COMMAND_LIST_TYPE_COPY, gDevice);
	}

	TE_RESULT DirectX12CommandQueue_Copy::ExecuteCommandList(DirectX12CommandList* cmdList)
	{
		/*auto c = cmdList->m_D3D12CommandList_Copy.Get();
		c->Close();

		ID3D12CommandList* l[1] = { c };
		m_CommandQueue->ExecuteCommandLists(1, l);

		cmdList->m_CommandAllocator_Copy.m_FenceValue = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().SetFence(m_CommandQueue.Get());
		cmdList->m_CommandAllocator_Copy.m_RunningCommandQueue = this;*/

		return TE_SUCCESSFUL;
	}

}