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
			case TE_RENDERER_API::DirectX12:
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


		std::vector<TruthEngine::Core::ShaderInputElement>* ShaderManager::GetInputElements(TE_IDX_SHADERCLASS shaderClassIDX)
		{
			auto& itr = m_Map_ShaderInputElements.find(shaderClassIDX);

			if (itr != m_Map_ShaderInputElements.end())
			{
				return &itr->second;
			}
			else
			{
				return CreateInputElements(shaderClassIDX);
			}

		}

		std::vector<TruthEngine::Core::ShaderInputElement>* ShaderManager::CreateInputElements(TE_IDX_SHADERCLASS shaderClassIDX)
		{
			auto& v = m_Map_ShaderInputElements[shaderClassIDX];
			ShaderInputElement inputElement;

			switch (shaderClassIDX)
			{
			case TE_IDX_SHADERCLASS::NONE:
				break;
			case TE_IDX_SHADERCLASS::FORWARDRENDERING:
				inputElement.AlignedByteOffset = 0;
				inputElement.Format = TE_RESOURCE_FORMAT::R32G32B32_FLOAT;
				inputElement.InputSlot = 0;
				inputElement.InputSlotClass = TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX;
				inputElement.InstanceDataStepRate = 0;
				inputElement.SemanticIndex = 0;
				inputElement.SemanticName = "POSITION";
				v.emplace_back(inputElement);

				inputElement.AlignedByteOffset = 0;
				inputElement.Format = TE_RESOURCE_FORMAT::R32G32B32_FLOAT;
				inputElement.InputSlot = 1;
				inputElement.InputSlotClass = TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX;
				inputElement.InstanceDataStepRate = 0;
				inputElement.SemanticIndex = 0;
				inputElement.SemanticName = "NORMAL";
				v.emplace_back(inputElement);

				inputElement.AlignedByteOffset = 12;
				inputElement.Format = TE_RESOURCE_FORMAT::R32G32B32_FLOAT;
				inputElement.InputSlot = 1;
				inputElement.InputSlotClass = TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX;
				inputElement.InstanceDataStepRate = 0;
				inputElement.SemanticIndex = 0;
				inputElement.SemanticName = "TANGENT";
				v.emplace_back(inputElement);

				inputElement.AlignedByteOffset = 24;
				inputElement.Format = TE_RESOURCE_FORMAT::R32G32_FLOAT;
				inputElement.InputSlot = 1;
				inputElement.InputSlotClass = TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX;
				inputElement.InstanceDataStepRate = 0;
				inputElement.SemanticIndex = 0;
				inputElement.SemanticName = "TEXCOORD";
				v.emplace_back(inputElement);
				break;
			default:
				throw;
				break;
			}

			return &v;
		}

		void ShaderManager::GetShaderDefines(const RendererStateSet states)
		{
			m_Defines.clear();
			if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE) == TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_TRUE)
			{
				m_Defines.emplace_back(L"ENABLE_MAP_DIFFUSE");
			}
			if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_NORMAL) == TE_RENDERER_STATE_ENABLED_MAP_NORMAL_TRUE)
			{
				m_Defines.emplace_back(L"ENABLE_MAP_NORMAL");
			}
			if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT) == TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_TRUE)
			{
				m_Defines.emplace_back(L"ENABLE_MAP_DISPLACEMENT");
			}
		}


	}
}