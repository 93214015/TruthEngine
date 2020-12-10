#include "pch.h"
#include "ShaderManager.h"

#include "API/DX12/DX12ShaderManager.h"

namespace TruthEngine
{

	namespace Core
	{		

		std::shared_ptr<TruthEngine::Core::ShaderManager> ShaderManager::Factory()
		{
				switch (Settings::RendererAPI)
				{
				case TE_RENDERER_API::DirectX12 :
					return std::make_shared<API::DirectX12::DX12ShaderManager>();
				default:
					return nullptr;
				}
		}


	}
}