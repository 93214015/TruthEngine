#pragma once

#include "Core/ImGui/ImGuiLayer.h"
#include "API/DX12/DirectX12DescriptorHeap.h"
#include "API/DX12/DirectX12Fence.h"
#include "API/DX12/DirectX12CommandList.h"


#ifdef TE_API_DX12


namespace TruthEngine::API::DirectX12
{
	class DirectX12ImGuiLayer : public TruthEngine::Core::ImGuiLayer
	{

	public:

		DirectX12ImGuiLayer() = default;

		void OnAttach() override;

		void OnDetach() override;

		void Begin() override;

		void End() override;

		void OnSceneViewportResize();

	private:
		DescriptorHeapSRV m_DescHeapSRV;
		DescriptorHeapRTV m_DescHeapRTV;
		std::shared_ptr<DirectX12CommandList> m_CommandList;

		uint32_t m_SRVIndexScreenBuffer = -1;
		ID3D12Resource* m_D3D12Resource_ScreenBuffer;

	};
}


#endif
