#include "pch.h"
#include "TextureMaterialManager.h"

#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/TextureMaterial.h"

#include "API/DX12/DirectX12TextureMaterialManager.h"

namespace TruthEngine
{
	uint32_t TextureMaterialManager::GetTextureViewIndex(uint32_t textureID) const
	{
		return m_Textures[textureID]->GetViewIndex();
	}
	std::shared_ptr<TextureMaterialManager> TextureMaterialManager::Factory()
	{

		{
			switch (Settings::RendererAPI)
			{
			case TE_RENDERER_API::DirectX12:
				return std::make_shared<API::DirectX12::DirectX12TextureMaterialManager>();
				break;
			default:
				break;
			}
		}

	}
}