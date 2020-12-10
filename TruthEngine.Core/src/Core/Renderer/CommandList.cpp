#include "pch.h"
#include "CommandList.h"

#include "API/DX12/DX12CommandList.h"

namespace TruthEngine::Core {



	std::shared_ptr<TruthEngine::Core::CommandList> CommandList::Factory(GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<ShaderManager> shaderManager)
	{
		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12 :
			return std::make_shared<API::DirectX12::DX12CommandList>(graphicDevice, type, bufferManager.get(), shaderManager.get());
		default:
			return nullptr;
		}
	}

}