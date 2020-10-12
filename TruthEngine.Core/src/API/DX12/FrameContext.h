#pragma once

namespace TruthEngine::API::DX12
{
	class FrameContext
	{
	public:

	private:
		std::vector<COMPTR<ID3D12CommandAllocator>> m_CommandAllocators;
		std::vector<COMPTR<ID3D12CommandAllocator>> m_Fences;
	};
}
