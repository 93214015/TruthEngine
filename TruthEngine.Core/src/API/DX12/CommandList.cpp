#include "pch.h"
#include "CommandList.h"
#include "GDeviceDX12.h"

namespace TruthEngine::API::DX12
{

	CommandList::CommandList() = default;


	void CommandList::Init(uint32_t FrameInFlyNum, D3D12_COMMAND_LIST_TYPE type, GDeviceDX12& gDevice)
	{
		m_CommandAllocators.reserve(FrameInFlyNum);
		for (uint32_t i = 0; i < FrameInFlyNum; ++i)
		{
			auto& cmdAlloc = m_CommandAllocators.emplace_back();
			cmdAlloc.Init(type, gDevice);
		}

		gDevice.CreateCommandList(m_CommandList, type);
	}

	void CommandList::Reset(uint32_t frameIndex, ID3D12PipelineState* pipelineState)
	{
		auto& cmdAlloc = m_CommandAllocators[frameIndex];

		cmdAlloc.Reset();

		m_CommandList->Reset(cmdAlloc.Get(), pipelineState);

		m_ActiveCommandAllocator = &cmdAlloc;
	}

}