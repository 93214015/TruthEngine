#include "pch.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "GDeviceDX12.h"

namespace TruthEngine::API::DX12 {



	CommandQueue::CommandQueue() = default;

	TE_RESULT CommandQueue::ExecuteCommandList(CommandList& cmdList) const
	{
		cmdList->Close();

		ID3D12CommandList* l[1] = { cmdList.Get() };
		m_CommandQueue->ExecuteCommandLists(1, l);

		auto fenceValue = TE_INSTANCE_API_DX12_GDEVICE.GetFence().SetFence(m_CommandQueue.Get());

		cmdList.GetActiveCommandAllocator()->m_FenceValue = fenceValue;

		return TE_SUCCESSFUL;
	}

	TE_RESULT CommandQueue::Init(D3D12_COMMAND_LIST_TYPE type, GDeviceDX12& gDevice)
	{
		return gDevice.CreateCommandQueue(m_CommandQueue, type);
	}

}