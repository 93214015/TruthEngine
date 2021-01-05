#include "pch.h"
#include "TextureMaterialManager.h"

#include "BufferManager.h"

#include "API/DX12/DirectX12TextureMaterialManager.h"

namespace TruthEngine 
{
	namespace Core
	{
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
}