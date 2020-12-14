#include "pch.h"
#include "CommandQueue.h"
#include "DX12CommandList.h"
#include "DX12GraphicDevice.h"

namespace TruthEngine::API::DirectX12 {



	CommandQueue::CommandQueue() = default;

	TE_RESULT CommandQueue::ExecuteCommandList(DX12CommandList* cmdList)
	{

		auto c = cmdList->GetNativeObject();
		c->Close();

		ID3D12CommandList* l[1] = { c };
		m_CommandQueue->ExecuteCommandLists(1, l);

		cmdList->m_CommandAllocator_Direct.m_FenceValue = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().SetFence(m_CommandQueue.Get());
		cmdList->m_CommandAllocator_Direct.m_RunningCommandQueue = this;

		return TE_SUCCESSFUL;

	}

	TE_RESULT CommandQueue::Init(D3D12_COMMAND_LIST_TYPE type, DX12GraphicDevice* gDevice)
	{
		return gDevice->CreateCommandQueue(m_CommandQueue, type);
	}

	TE_RESULT CommandQueue_Direct::Init(DX12GraphicDevice* gDevice)
	{
		return CommandQueue::Init(D3D12_COMMAND_LIST_TYPE_DIRECT, gDevice);
	}

	TE_RESULT CommandQueue_Direct::ExecuteCommandList(DX12CommandList* cmdList)
	{
		auto c = cmdList->m_D3D12CommandList_Direct.Get();
		c->Close();

		ID3D12CommandList* l[1] = { c };
		m_CommandQueue->ExecuteCommandLists(1, l);

		cmdList->m_CommandAllocator_Direct.m_FenceValue = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().SetFence(m_CommandQueue.Get());
		cmdList->m_CommandAllocator_Direct.m_RunningCommandQueue = this;

		return TE_SUCCESSFUL;
	}

	TE_RESULT CommandQueue_Copy::Init(DX12GraphicDevice* gDevice)
	{
		return CommandQueue::Init(D3D12_COMMAND_LIST_TYPE_COPY, gDevice);
	}

	TE_RESULT CommandQueue_Copy::ExecuteCommandList(DX12CommandList* cmdList)
	{
		auto c = cmdList->m_D3D12CommandList_Copy.Get();
		c->Close();

		ID3D12CommandList* l[1] = { c };
		m_CommandQueue->ExecuteCommandLists(1, l);

		cmdList->m_CommandAllocator_Copy.m_FenceValue = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFence().SetFence(m_CommandQueue.Get());
		cmdList->m_CommandAllocator_Copy.m_RunningCommandQueue = this;

		return TE_SUCCESSFUL;
	}

}