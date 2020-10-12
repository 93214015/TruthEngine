#pragma once

#include "Core/ImGui/ImGuiLayer.h"
#include "API/DX12/DescriptorHeap.h"
#include "API/DX12/Fence.h"
#include "API/DX12/CommandList.h"


#ifdef TE_API_DX12


namespace TruthEngine::API::DX12
{
	class ImGuiLayerDX12 : public TruthEngine::Core::ImGuiLayer
	{

	public:
		void OnAttach() override;


		void OnDetach() override;


		void OnUpdate(double deltaFrameTime) override;


		void OnImGuiRender() override;


		void OnEvent(TruthEngine::Core::Event& event) override;

		void Begin();
		void End();

	private:
		DescriptorHeapSRV m_DescHeapSRV;
		CommandList m_CommandList;

	};
}


#endif
