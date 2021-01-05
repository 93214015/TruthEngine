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

		Shader* ShaderManager::GetShader(TE_IDX_SHADERCLASS shaderClassID, RendererStateSet states)
		{
			{
				states &= m_StateMask;

				auto& map = m_ShadersStateMap[static_cast<uint32_t>(shaderClassID)];

				auto itr = map.find(states);

				if (itr != m_ShadersStateMap->end())
				{
					return itr->second.get();
				}

				return nullptr;
			}
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
				/*m_Map_BindedResources[shaderClassIDX] = BindedResource{
					{{ TE_IDX_CONSTANTBUFFER::PER_FRAME, TE_IDX_CONSTANTBUFFER::PER_DLIGHT, TE_IDX_CONSTANTBUFFER::MATERIALS } , { TE_IDX_CONSTANTBUFFER::DIRECT_PER_MESH }},
					{{ TE_IDX_TEXTURE::MATERIALTEXTURE_DIFFUSE }, {TE_IDX_TEXTURE::GBUFFER_NORMAL}}
				};*/
				m_Map_BindedResources[shaderClassIDX] = BindedResource{
					{{ {0, 0, TE_IDX_CONSTANTBUFFER::PER_FRAME}, {1, 0, TE_IDX_CONSTANTBUFFER::PER_DLIGHT}, {2, 0, TE_IDX_CONSTANTBUFFER::MATERIALS} } , { {3, 0, TE_IDX_CONSTANTBUFFER::DIRECT_PER_MESH} } },
					{{ {0, 0, TE_IDX_TEXTURE::MATERIALTEXTURE_DIFFUSE} }, {{ 0, 1, TE_IDX_TEXTURE::MATERIALTEXTURE_NORMAL }}}
				};
				return &m_Map_BindedResources[shaderClassIDX];
			default:
				break;
			}

		}

	}
}