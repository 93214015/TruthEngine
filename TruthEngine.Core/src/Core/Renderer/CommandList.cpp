#include "pch.h"
#include "CommandList.h"

#include "API/DX12/DirectX12CommandList.h"

namespace TruthEngine::Core {



	std::shared_ptr<TruthEngine::Core::CommandList> CommandList::Factory(GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<ShaderManager> shaderManager, TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX)
	{
		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12 :
			return std::make_shared<API::DirectX12::DirectX12CommandList>(graphicDevice, type, bufferManager.get(), shaderManager.get(), renderPassIDX, shaderClassIDX);
		default:
			return nullptr;
		}
	}

	CommandList::CommandList(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX)
		: m_ShaderClassIDX(shaderClassIDX), m_RenderPassIDX(renderPassIDX)
	{
	}

}