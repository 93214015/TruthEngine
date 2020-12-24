#include "pch.h"
#include "ShaderManager.h"

#include "API/DX12/DirectX12ShaderManager.h"

namespace TruthEngine
{

	namespace Core
	{
		BindedResource* ShaderManager::GetBindedResource(const TE_IDX_SHADERCLASS shaderClassIDX)
		{
			auto& itr = m_Map_BindedResources.find(shaderClassIDX);
			if (itr != m_Map_BindedResources.end())
			{
				return &itr->second;
			}

			return CreateBindedResource(shaderClassIDX);
		}
		std::shared_ptr<TruthEngine::Core::ShaderManager> ShaderManager::Factory()
		{
				switch (Settings::RendererAPI)
				{
				case TE_RENDERER_API::DirectX12 :
					return std::make_shared<API::DirectX12::DirectX12ShaderManager>();
				default:
					return nullptr;
				}
		}

		BindedResource* ShaderManager::CreateBindedResource(const TE_IDX_SHADERCLASS shaderClassIDX)
		{
			switch (shaderClassIDX)
			{
			case TE_IDX_SHADERCLASS::FORWARDRENDERING:
				m_Map_BindedResources[shaderClassIDX] = BindedResource{
					{{ TE_IDX_CONSTANTBUFFER::PER_FRAME }},
					{{}}
				};
				return &m_Map_BindedResources[shaderClassIDX];
			default:
				break;
			}

		}

	}
}