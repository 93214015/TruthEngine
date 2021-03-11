#include "pch.h"
#include "ShaderManager.h"

#include "API/DX12/DirectX12ShaderManager.h"
#include "BufferManager.h"

namespace TruthEngine
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


	const TruthEngine::ShaderRequiredResources* ShaderManager::GetShaderRequiredResources(const TE_IDX_SHADERCLASS _ShaderClassIDX) const
	{
		if (_ShaderClassIDX == TE_IDX_SHADERCLASS::NONE)
			return nullptr;

		auto _Itr = m_Map_ShaderRequiedResources.find(_ShaderClassIDX);

		TE_ASSERT_CORE(_Itr != m_Map_ShaderRequiedResources.end(), "ShaderManager::GetShaderRequiredResources : ShaderRequiredResource was not found!");

		return &_Itr->second;
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


	std::shared_ptr<TruthEngine::ShaderManager> ShaderManager::Factory()
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

		ShaderSignature* _ShaderSignature = &m_Map_ShaderSignatures[shaderClassIDX];

		switch (shaderClassIDX)
		{
		case TE_IDX_SHADERCLASS::NONE:
			break;
		case TE_IDX_SHADERCLASS::FORWARDRENDERING:
		case TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_PerMesh, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL));

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL)
				, {
						ShaderSignature::ShaderRange(1, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_LightData, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_Materials, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_UnFrequent, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_ShadowMap, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_CubeMap_Environment, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(2, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_MaterialTextures, ShaderSignature::EShaderRangeType::SRV)
						} , 500)
				}
			);

			break;
		}
		case TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_PIXEL_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_ShadowMapPerMesh, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_VERTEX));

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_ShadowMapPerLight, 1, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_VERTEX));
			break;
		}
		case TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_FIRSTPASS:
		{
			_ShaderSignature->mShaderSignatureFlags = 
				ShaderSignature::EShaderSignatureFlags_DENY_VERTEX_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_PIXEL_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL)
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_HDR_DownScaling, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::UAV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, ShaderSignature::EShaderRangeType::UAV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);


			break;
		}
		case TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_SECONDPASS:
		{
			_ShaderSignature->mShaderSignatureFlags = 
				ShaderSignature::EShaderSignatureFlags_DENY_VERTEX_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_PIXEL_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL)
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_HDR_DownScaling, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::UAV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass, ShaderSignature::EShaderRangeType::UAV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);

			break;
		}
		case TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_FINALPASS:
		{
			_ShaderSignature->mShaderSignatureFlags =
				ShaderSignature::EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL)
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_HDR_FinalPass, ShaderSignature::EShaderRangeType::CBV),
						}),						
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);

			break;
		}
		default:
			throw;
			break;
		}

		ShaderRequiredResources* _ShaderRequiredResources = &m_Map_ShaderRequiedResources[shaderClassIDX];
		_ShaderRequiredResources->AddResource(_ShaderSignature);

		_CreateInputElements(_ShaderSignature->InputElements, shaderClassIDX);

		return _ShaderSignature;

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
		case TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP:
		{
			auto& ie = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_NTT];
			ie.emplace_back("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			break;
		}
		default:
			break;
		}

	}

	/*

	std::vector<std::vector<ShaderSignature::ShaderConstantBufferViewSlot>> ShaderManager::_CreateConstantBufferViewSlots(TE_IDX_SHADERCLASS shaderClassIDX)
	{
		std::vector<std::vector<ShaderSignature::ShaderConstantBufferViewSlot>> v;

		switch (shaderClassIDX)
		{
		case TE_IDX_SHADERCLASS::NONE:
			break;
		case TE_IDX_SHADERCLASS::FORWARDRENDERING:
		case TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP:
		{
			v =
			{
				{ ShaderSignature::ShaderConstantBufferViewSlot{0, 0, TE_IDX_GRESOURCES::CBuffer_PerFrame}, ShaderSignature::ShaderConstantBufferViewSlot{1, 0, TE_IDX_GRESOURCES::CBuffer_LightData}, ShaderSignature::ShaderConstantBufferViewSlot{2, 0, TE_IDX_GRESOURCES::CBuffer_Materials}, ShaderSignature::ShaderConstantBufferViewSlot{3, 0, TE_IDX_GRESOURCES::CBuffer_UnFrequent} },
				{ ShaderSignature::ShaderConstantBufferViewSlot{4, 0, TE_IDX_GRESOURCES::Constant_PerMesh} }
			};
			break;
		}
		case TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP:
			v =
			{
				{ ShaderSignature::ShaderConstantBufferViewSlot{0, 0, TE_IDX_GRESOURCES::Constant_ShadowMapPerLight} },
				{ ShaderSignature::ShaderConstantBufferViewSlot{1, 0, TE_IDX_GRESOURCES::Constant_ShadowMapPerMesh} }
			};
			break;
		default:
			throw;
			break;
		}
		return v;
	}


	std::vector<std::vector<ShaderSignature::ShaderShaderResourceViewSlot>> ShaderManager::_CreateShaderResourceViewSlots(TE_IDX_SHADERCLASS shaderClassIDX)
	{
		std::vector<std::vector<ShaderSignature::ShaderShaderResourceViewSlot>> v;

		switch (shaderClassIDX)
		{
		case TE_IDX_SHADERCLASS::NONE:
			break;
		case TE_IDX_SHADERCLASS::FORWARDRENDERING:
		case TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP:
		{
			v =
			{
				{ ShaderSignature::ShaderShaderResourceViewSlot(0, 0, TE_RESOURCE_TYPE::TEXTURE2D, (IDX)TE_IDX_GRESOURCES::Texture_DS_ShadowMap), ShaderSignature::ShaderShaderResourceViewSlot{1, 0, TE_RESOURCE_TYPE::TEXTURE2D, (IDX)TE_IDX_GRESOURCES::Texture_CubeMap_Environment} },
				{ ShaderSignature::ShaderShaderResourceViewSlot{2, 0, TE_RESOURCE_TYPE::TEXTURE2D, (IDX)TE_IDX_GRESOURCES::Texture_MaterialTextures} },
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
	}*/


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