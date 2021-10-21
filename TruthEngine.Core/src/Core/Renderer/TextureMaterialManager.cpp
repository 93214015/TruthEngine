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

	TextureMaterialManager* TextureMaterialManager::Factory()
	{
		switch (Settings::Graphics::GetRendererAPI())
		{
		case Settings::Graphics::TE_RENDERER_API::DirectX12:
			return API::DirectX12::DirectX12TextureMaterialManager::GetInstance();
			break;
		default:
			break;
		}

	}
}