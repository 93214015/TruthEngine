#pragma once
#include "CommandAllocator.h"

namespace TruthEngine::API::DX12
{

	class CommandList {
		friend class GraphicDeviceDX12;
	public:
		CommandList();

		void Init(uint32_t FrameInflyNum, D3D12_COMMAND_LIST_TYPE type, GraphicDeviceDX12& gDevice);

		inline ID3D12GraphicsCommandList* Get() { return m_CommandList.Get(); }

		ID3D12GraphicsCommandList* operator ->() { return m_CommandList.Get(); }

		inline CommandAllocator* GetActiveCommandAllocator() const { return m_ActiveCommandAllocator; }

		void Reset(uint32_t frameIndex, ID3D12PipelineState* pipelineState);

		inline void Release() {
			for (auto& cmdAlloc : m_CommandAllocators)
			{
				cmdAlloc.Release();
			}
			m_CommandList.Reset();
		}


	private:


	private:

		COMPTR<ID3D12GraphicsCommandList> m_CommandList;
		std::vector<CommandAllocator> m_CommandAllocators;

		CommandAllocator* m_ActiveCommandAllocator = nullptr;


	};


}
