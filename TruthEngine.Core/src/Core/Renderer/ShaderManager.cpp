#include "pch.h"
#include "ShaderManager.h"

#include "API/DX12/DirectX12ShaderManager.h"

namespace TruthEngine
{

	namespace Core
	{

		ShaderSignature* ShaderManager::GetShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX)
		{
			if (shaderClassIDX == TE_IDX_SHADERCLASS::NONE)
				return nullptr;

			auto& itr = m_Map_ShaderSignatures.find(shaderClassIDX);
			if (itr != m_Map_ShaderSignatures.end())
			{
				return &itr->second;
			}

			return _CreateShaderSignature(shaderClassIDX);
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


		ShaderSignature* ShaderManager::_CreateShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX)
		{
			ShaderSignature* sg;

			switch (shaderClassIDX)
			{
			case TE_IDX_SHADERCLASS::FORWARDRENDERING:
			{
				sg = &m_Map_ShaderSignatures[shaderClassIDX];
				sg->ConstantBuffers = _CreateConstantBufferSlots(shaderClassIDX);
				sg->Textures = _CreateTextureSlots(shaderClassIDX);
				_CreateInputElements(sg->InputElements, shaderClassIDX);
				break;
			}
			case TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP:
			{
				sg = &m_Map_ShaderSignatures[shaderClassIDX];
				sg->ConstantBuffers = _CreateConstantBufferSlots(shaderClassIDX);
				sg->Textures = _CreateTextureSlots(shaderClassIDX);
				_CreateInputElements(sg->InputElements, shaderClassIDX);
				break;
			}
			default:
				throw;
				break;
			}

			return sg;

		}


		void ShaderManager::_CreateInputElements(std::vector<ShaderInputElement> shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::TOTALNUM], TE_IDX_SHADERCLASS shaderClassIDX)
		{

			ShaderInputElement inputElement;

			switch (shaderClassIDX)
			{
			case TE_IDX_SHADERCLASS::NONE:
				break;
			case TE_IDX_SHADERCLASS::FORWARDRENDERING:
			{
				auto& ie = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_NTT];
				ie.emplace_back("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
				ie.emplace_back("NORMAL", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
				ie.emplace_back("TANGENT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
				ie.emplace_back("TEXCOORD", 0, TE_RESOURCE_FORMAT::R32G32_FLOAT, 1, 24, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
				break;
			}
			case TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP:
			{
				auto& ie = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_NTT];
				ie.emplace_back("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
				break;
			}
			default:
				throw;
				break;
			}

		}


		std::vector<std::vector<ShaderSignature::ShaderConstantBufferSlot>> ShaderManager::_CreateConstantBufferSlots(TE_IDX_SHADERCLASS shaderClassIDX)
		{
			std::vector<std::vector<ShaderSignature::ShaderConstantBufferSlot>> v;

			switch (shaderClassIDX)
			{
			case TE_IDX_SHADERCLASS::NONE:
				break;
			case TE_IDX_SHADERCLASS::FORWARDRENDERING:
			{
				v =
				{
					{ ShaderSignature::ShaderConstantBufferSlot{0, 0, TE_IDX_CONSTANTBUFFER::PER_FRAME}, ShaderSignature::ShaderConstantBufferSlot{1, 0, TE_IDX_CONSTANTBUFFER::PER_DLIGHT}, ShaderSignature::ShaderConstantBufferSlot{2, 0, TE_IDX_CONSTANTBUFFER::MATERIALS} },
					{ ShaderSignature::ShaderConstantBufferSlot{3, 0, TE_IDX_CONSTANTBUFFER::DIRECT_PER_MESH} }
				};
				break;
			}
			case TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP:
				v =
				{
					{ ShaderSignature::ShaderConstantBufferSlot{0, 0, TE_IDX_CONSTANTBUFFER::DIRECT_SHADOWMAP_PER_LIGHT} },
					{ ShaderSignature::ShaderConstantBufferSlot{1, 0, TE_IDX_CONSTANTBUFFER::DIRECT_SHADOWMAP_PER_MESH} }
				};
				break;
			default:
				throw;
				break;
			}
			return v;
		}


		std::vector<std::vector<ShaderSignature::ShaderTextureSlot>> ShaderManager::_CreateTextureSlots(TE_IDX_SHADERCLASS shaderClassIDX)
		{
			std::vector<std::vector<ShaderSignature::ShaderTextureSlot>> v;

			switch (shaderClassIDX)
			{
			case TE_IDX_SHADERCLASS::NONE:
				break;
			case TE_IDX_SHADERCLASS::FORWARDRENDERING:
			{
				v =
				{ 
					{ ShaderSignature::ShaderTextureSlot{0, 0, TE_IDX_TEXTURE::DS_SHADOWMAP} },
					{ ShaderSignature::ShaderTextureSlot{1, 0, TE_IDX_TEXTURE::MATERIALTEXTURES} },
				};
				break;
			}
			case TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP:
				break;
			default:
				throw;
				break;
			}

			return v;
		}


		void ShaderManager::_GetShaderDefines(const RendererStateSet states)
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