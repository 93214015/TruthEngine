#include "pch.h"
#include "ImGuiLayer.h"

#include "API/DX12/DX12ImGuiLayer.h"

namespace TruthEngine::Core
{



	std::shared_ptr<TruthEngine::Core::ImGuiLayer> ImGuiLayer::Factory()
	{
		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12:
			return std::make_shared<API::DirectX12::DX12ImGuiLayer>();
		default:
			return nullptr;
		}
	}

}