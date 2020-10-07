#pragma once
#include "Core/Layer.h"
#include "API/DX12/DescriptorHeap.h"

namespace TruthEngine::Core
{

	class ImGuiLayer: public Layer
	{


	public:
		void OnAttach() override;


		void OnDetach() override;


		void OnUpdate(double deltaFrameTime) override;


		void OnImGuiRender() override;


		void OnEvent(const Event& event) override;

	private:
		#ifdef TE_API_DX12
			TruthEngine::API::DX12::DescriptorHeapSRV m_DescHeapSRV;
		#endif

	};
}
