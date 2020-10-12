#pragma once
#include "Core/Layer.h"
#include "API/DX12/DescriptorHeap.h"

namespace TruthEngine::Core
{

	class ImGuiLayer: public Layer
	{


	public:

		static ImGuiLayer* Factory();


		virtual ~ImGuiLayer();


		void OnAttach() override = 0;


		void OnDetach() override = 0;


		void OnUpdate(double deltaFrameTime) override = 0;


		void OnImGuiRender() override = 0;


		void OnEvent(TruthEngine::Core::Event& event) override = 0;

	protected:
		ImGuiLayer() = default;


	};
}
