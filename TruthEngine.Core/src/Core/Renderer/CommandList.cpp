#include "pch.h"
#include "CommandList.h"

#include "API/DX12/DirectX12CommandList.h"

#include "ShaderManager.h"

namespace TruthEngine {



	std::shared_ptr<TruthEngine::CommandList> CommandList::Factory(GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, BufferManager* bufferManager, ShaderManager* shaderManager, TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint8_t frameIndex)
	{
		switch (Settings::Graphics::GetRendererAPI())
		{
		case TE_RENDERER_API::DirectX12 :
			return std::make_shared<API::DirectX12::DirectX12CommandList>(graphicDevice, type, bufferManager, shaderManager, renderPassIDX, shaderClassIDX, frameIndex);
		default:
			return nullptr;
		}
	}

	CommandList::CommandList(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint8_t frameIndex)
		:  m_RenderPassIDX(renderPassIDX), m_ShaderClassIDX(shaderClassIDX), m_FrameIndex(frameIndex)
	{
	}

}