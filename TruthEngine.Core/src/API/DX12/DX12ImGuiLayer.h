#pragma once

#include "Core/ImGui/ImGuiLayer.h"
#include "API/DX12/DescriptorHeap.h"
#include "API/DX12/Fence.h"
#include "API/DX12/DX12CommandList.h"


#ifdef TE_API_DX12


namespace TruthEngine::API::DirectX12
{
	class DX12ImGuiLayer : public TruthEngine::Core::ImGuiLayer
	{

	public:

		DX12ImGuiLayer() = default;

		void OnAttach() override;

		void OnDetach() override;

		void Begin() override;

		void End() override;

	private:
		DescriptorHeapSRV m_DescHeapSRV;
		DescriptorHeapRTV m_DescHeapRTV;
		std::shared_ptr<DX12CommandList> m_CommandList;

	};
}


#endif
