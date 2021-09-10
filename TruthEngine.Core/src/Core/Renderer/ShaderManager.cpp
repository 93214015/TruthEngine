#include "pch.h"
#include "ShaderManager.h"

#include "API/DX12/DirectX12ShaderManager.h"
#include "BufferManager.h"

constexpr uint32_t MATERIAL_TEXTURE_COUNT = 500;

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


	Shader* ShaderManager::GetShader(TE_IDX_SHADERCLASS shaderClassID, TE_IDX_MESH_TYPE _MeshType, RendererStateSet states)
	{
		{
			states &= m_StateMask;

			auto& map = m_ShadersStateMap[static_cast<uint32_t>(shaderClassID)][static_cast<uint32_t>(_MeshType)];

			auto itr = map.find(states);

			if (itr != map.end())
			{
				return itr->second.get();
			}

			return nullptr;
		}
	}


	TruthEngine::ShaderManager* ShaderManager::Factory()
	{
		switch (Settings::GetRendererAPI())
		{
		case TE_RENDERER_API::DirectX12:
			return API::DirectX12::DirectX12ShaderManager::GetInstance();
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
		case TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_EnvironmentMap, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL));

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL)
				, {
						ShaderSignature::ShaderRange(1, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_CubeMap_Environment, ShaderSignature::EShaderRangeType::SRV),
							//ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_CubeMap, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);

			break;
		}
		case TE_IDX_SHADERCLASS::FORWARDRENDERING:
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
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_Bones, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_ShadowMap_SunLight, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_ShadowMap_SpotLight, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_CubeMap_Environment, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_CubeMap_IBLAmbient, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_CubeMap_IBLSpecular, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_PrecomputedBRDF, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(6, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_MaterialTextures, ShaderSignature::EShaderRangeType::SRV)
						} , MATERIAL_TEXTURE_COUNT)
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

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_VERTEX)
				, {
						ShaderSignature::ShaderRange(2, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_Bones, ShaderSignature::EShaderRangeType::CBV),
						})						
				}
			);
			break;
		}
		case TE_IDX_SHADERCLASS::RENDERBOUNDINGBOX:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_RenderBoundingBox, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_GEOMETRY));
						
			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_GEOMETRY)
				, {
						ShaderSignature::ShaderRange(1, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
						})
				}
			);
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
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_DownScaledHDR, ShaderSignature::EShaderRangeType::UAV),
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
			_ShaderParam->mParameter = ShaderSignature::ShaderDirectView(
				0,
				0,
				ShaderSignature::EShaderDirectViewFlags::EShaderResourceViewFlag_NONE,
				ShaderSignature::EShaderVisibility_ALL,
				ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_UAV_Buffer);

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderDirectView(
				0,
				0,
				ShaderSignature::EShaderDirectViewFlags::EShaderResourceViewFlag_NONE,
				ShaderSignature::EShaderVisibility_ALL,
				ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_SRV_Buffer);

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL)
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_HDR_DownScaling, ShaderSignature::EShaderRangeType::CBV),
						}),
						/*ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::UAV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass, ShaderSignature::EShaderRangeType::UAV),
						}),*/
						ShaderSignature::ShaderRange(1, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);


			break;
		}
		case TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_BLOOMPASS:
		{
			_ShaderSignature->mShaderSignatureFlags =
				ShaderSignature::EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;


			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderDirectView(
				1,
				0,
				ShaderSignature::EShaderDirectViewFlags::EShaderResourceViewFlag_NONE,
				ShaderSignature::EShaderVisibility_ALL,
				ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_SRV_Buffer);

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				ShaderSignature::EShaderVisibility_ALL
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_HDR_DownScaling, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_DownScaledHDR, ShaderSignature::EShaderRangeType::SRV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::UAV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_Bloom, ShaderSignature::EShaderRangeType::UAV),
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
			_ShaderParam->mParameter = ShaderSignature::ShaderDirectView(
				4,
				0,
				ShaderSignature::EShaderDirectViewFlags::EShaderResourceViewFlag_NONE,
				ShaderSignature::EShaderVisibility_PIXEL,
				ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_SRV_Buffer);

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				ShaderSignature::EShaderVisibility_PIXEL
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_HDR_FinalPass, ShaderSignature::EShaderRangeType::CBV),
						}),						
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_DownScaledHDR, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_BloomBlured, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, ShaderSignature::EShaderRangeType::SRV),
							//ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);


			break;
		}
		case TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_HORZ:
		{
			_ShaderSignature->mShaderSignatureFlags =
				ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				ShaderSignature::EShaderVisibility_ALL
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_GaussianBlur, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_Bloom, ShaderSignature::EShaderRangeType::SRV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::UAV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_BloomBluredHorz, ShaderSignature::EShaderRangeType::UAV),
						})
				}
			);


			break;
		}
		case TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_VERT:
		{
			_ShaderSignature->mShaderSignatureFlags =
				ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				ShaderSignature::EShaderVisibility_ALL
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_GaussianBlur, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_BloomBluredHorz, ShaderSignature::EShaderRangeType::SRV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::UAV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RW_BloomBlured, ShaderSignature::EShaderRangeType::UAV),
						})
				}
			);


			break;
		}
		case TE_IDX_SHADERCLASS::GENERATEGBUFFERS:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_PerMesh_GBuffers, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL));

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL)
				, {
						ShaderSignature::ShaderRange(1, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_Materials, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_Bones, ShaderSignature::EShaderRangeType::CBV),
						})
				}
			);

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_MaterialTextures, ShaderSignature::EShaderRangeType::SRV)
						} , MATERIAL_TEXTURE_COUNT)
				}
			);

			break;
		}
		case TE_IDX_SHADERCLASS::DEFERREDSHADING:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;


			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_LightData, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_UnFrequent, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Color, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Normal, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Specular, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_CubeMap_IBLAmbient, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_CubeMap_IBLSpecular, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_PrecomputedBRDF, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_SSAOBlurred, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_ShadowMap_SunLight, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_ShadowMap_SpotLight, ShaderSignature::EShaderRangeType::SRV),
						})
				}

			);

			break;
		}
		case TE_IDX_SHADERCLASS::GENERATECUBEMAP:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_ALLOW_STREAM_OUTPUT
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;


			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_InputCreateCubeMap, ShaderSignature::EShaderRangeType::SRV),
						})
				}

			);

			break;
		}
		case TE_IDX_SHADERCLASS::GENERATEIBLAMBIENT:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_ALLOW_STREAM_OUTPUT
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;


			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_InputCreateIBLAmbient, ShaderSignature::EShaderRangeType::SRV),
						})
				}

			);

			break;
		}
		case TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR_PREFILTER:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_ALLOW_STREAM_OUTPUT
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_IBL_Specular, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_PIXEL));

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_InputCreateIBLSpecular, ShaderSignature::EShaderRangeType::SRV),
						})
				}

			);

			break;
		}
		case TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR_BRDF:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			break;
		}
		case TE_IDX_SHADERCLASS::GENERATESSAO:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;


			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_UnFrequent, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_SSAO, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Normal, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_SSAONoises, ShaderSignature::EShaderRangeType::SRV),
						})
				}

			);

			break;
		}
		case TE_IDX_SHADERCLASS::SSAOBLURRING:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;


			auto _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_SSAO, ShaderSignature::EShaderRangeType::SRV),
						})
				}

			);

			break;
		}
		case TE_IDX_SHADERCLASS::RENDERENTITYICON:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_RenderEntityIcon, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL));

			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_GEOMETRY)
				, {
						ShaderSignature::ShaderRange(1, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
						})
				}
			);


			_ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_PIXEL),
				{
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_MaterialTextures, ShaderSignature::EShaderRangeType::SRV)
						} , MATERIAL_TEXTURE_COUNT)
				}
			);

			break;
		}
		case TE_IDX_SHADERCLASS::WIREFRAME:
		{
			_ShaderSignature->mShaderSignatureFlags = ShaderSignature::EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			ShaderSignature::ShaderParameter* _ShaderParam = &_ShaderSignature->AddParameter();
			_ShaderParam->mParameter = ShaderSignature::ShaderConstant(TE_IDX_GRESOURCES::Constant_Wireframe, 0, 0, static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_ALL));


			break;
		}
		case TE_IDX_SHADERCLASS::SSREFLECTION:
		{
			_ShaderSignature->mShaderSignatureFlags = 
				ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			auto& _ShaderParam = _ShaderSignature->AddParameter();
			_ShaderParam.mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_PIXEL)
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::CBV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_PerFrame, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_UnFrequent, ShaderSignature::EShaderRangeType::CBV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::CBuffer_SSReflection, ShaderSignature::EShaderRangeType::CBV),
						}),
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_GBuffer_PosView, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Specular, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Normal, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, ShaderSignature::EShaderRangeType::SRV),
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);

			break;
		}
		case TE_IDX_SHADERCLASS::BLENDREFLECTION:
		{
			_ShaderSignature->mShaderSignatureFlags =
				ShaderSignature::EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS
				| ShaderSignature::EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS;

			auto& _ShaderParam = _ShaderSignature->AddParameter();
			_ShaderParam.mParameter = ShaderSignature::ShaderTable
			(
				static_cast<ShaderSignature::EShaderVisibility>(ShaderSignature::EShaderVisibility_PIXEL)
				, {
						ShaderSignature::ShaderRange(0, 0, ShaderSignature::EShaderRangeType::SRV, ShaderSignature::EShaderRangeFlags::EShaderRangeFlags_NONE,
						{
							ShaderSignature::ShaderRangeView(TE_IDX_GRESOURCES::Texture_RT_SSReflection, ShaderSignature::EShaderRangeType::SRV),
						})
				}
			);


			break;
		}
		default:
			TE_ASSERT_CORE(false, "There is no Shader Signature for specifid shader idx");
			break;
		}

		ShaderRequiredResources* _ShaderRequiredResources = &m_Map_ShaderRequiedResources[shaderClassIDX];
		_ShaderRequiredResources->AddResource(_ShaderSignature);

		_CreateInputElements(_ShaderSignature->InputElements, shaderClassIDX);

		return _ShaderSignature;

	}


	void ShaderManager::_CreateInputElements(std::vector<ShaderInputElement> shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::TOTALNUM], TE_IDX_SHADERCLASS shaderClassIDX)
	{
		switch (shaderClassIDX)
		{
		case TE_IDX_SHADERCLASS::NONE:
			break;
		case TE_IDX_SHADERCLASS::FORWARDRENDERING:
		case TE_IDX_SHADERCLASS::GENERATEGBUFFERS:
		{
			auto& ie = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_NTT];
			ie.emplace_back("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie.emplace_back("NORMAL", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie.emplace_back("TANGENT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie.emplace_back("TEXCOORD", 0, TE_RESOURCE_FORMAT::R32G32_FLOAT, 1, 24, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);

			auto& ie2 = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_SKINNED];
			ie2.emplace_back("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie2.emplace_back("NORMAL", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie2.emplace_back("TANGENT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie2.emplace_back("TEXCOORD", 0, TE_RESOURCE_FORMAT::R32G32_FLOAT, 1, 24, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie2.emplace_back("BONEWEIGHT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 2, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie2.emplace_back("BONEINDEX", 0, TE_RESOURCE_FORMAT::R8G8B8A8_UINT, 2, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);

			break;
		}
		case TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP:
		case TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP:
		{
			auto& ie = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_NTT];
			ie.emplace_back("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);

			auto& ie2 = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_SKINNED];
			ie2.emplace_back("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie2.emplace_back("BONEWEIGHT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 2, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			ie2.emplace_back("BONEINDEX", 0, TE_RESOURCE_FORMAT::R8G8B8A8_UINT, 2, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0);
			break;
		}
		case TE_IDX_SHADERCLASS::WIREFRAME:
		{
			auto& ie = shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::MESH_SIMPLE];
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
				{ ShaderSignature::ShaderShaderResourceViewSlot(0, 0, TE_RESOURCE_TYPE::TEXTURE2D, (IDX)TE_IDX_GRESOURCES::Texture_DS_ShadowMap_SunLight), ShaderSignature::ShaderShaderResourceViewSlot{1, 0, TE_RESOURCE_TYPE::TEXTURE2D, (IDX)TE_IDX_GRESOURCES::Texture_CubeMap_Environment} },
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


	void ShaderManager::_GetShaderDefines(const RendererStateSet states, TE_IDX_MESH_TYPE _MeshType)
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
		if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_SPECULAR) == TE_RENDERER_STATE_ENABLED_MAP_SPECULAR_TRUE)
		{
			m_Defines.emplace_back(L"ENABLE_MAP_SPECULAR");
		}
		if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS) == TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS_TRUE)
		{
			m_Defines.emplace_back(L"ENABLE_MAP_ROUGHNESS");
		}
		if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_METALLIC) == TE_RENDERER_STATE_ENABLED_MAP_METALLIC_TRUE)
		{
			m_Defines.emplace_back(L"ENABLE_MAP_METALLIC");
		}
		if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION) == TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION_TRUE)
		{
			m_Defines.emplace_back(L"ENABLE_MAP_AMBIENTOCCLUSION");
		}
		if (GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_HDR) == TE_RENDERER_STATE_ENABLED_HDR_TRUE)
		{
			m_Defines.emplace_back(L"ENABLE_HDR");
		}


		switch (_MeshType)
		{
		case TE_IDX_MESH_TYPE::MESH_POINT:
			m_Defines.emplace_back(L"MESH_TYPE_POINT");
			break;
		case TE_IDX_MESH_TYPE::MESH_SIMPLE:
			m_Defines.emplace_back(L"MESH_TYPE_SIMPLE");
			break;
		case TE_IDX_MESH_TYPE::MESH_NTT:
			m_Defines.emplace_back(L"MESH_TYPE_NTT");
			break;
		case TE_IDX_MESH_TYPE::MESH_SKINNED:
			m_Defines.emplace_back(L"MESH_TYPE_SKINNED");
			break;
		default:
			break;
		}
	}



}