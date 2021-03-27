#pragma once
#include "Material.h"

namespace TruthEngine
{
	class BufferManager;
	class RendererCommand;

	class MaterialManager
	{
	public:
		MaterialManager();

		inline Material* GetMaterial(const uint32_t materialID)
		{
			return m_Map_Materials[materialID].get();
		}

		inline const std::vector<Material*>& GetMaterials() const
		{
			return m_Materials;
		}

		inline Material* GetDefaultMaterial(uint32_t index)
		{
			return m_Map_Materials[index].get();
		}

		inline size_t GetMatrialOffset() const noexcept
		{
			return m_Map_Materials.size();
		}

		Material* AddDefaultMaterial(TE_IDX_MESH_TYPE meshType);

		Material* AddMaterial(
			RendererStateSet states
			, const float4& colorDiffuse
			, const float3& fresnelR0
			, float shininess
			, const float2& uvScale
			, const float2& uvTranslate
			, uint32_t diffuseMapIndex
			, uint32_t normalMapIndex
			, uint32_t displacementMapIndex
			, int32_t extraDepthBias
			, float extraSlopeScaledDepthBias
			, float extraDepthBiasClamp
			, TE_IDX_MESH_TYPE meshType);

		Material* AddMaterial(Material* material);

		static MaterialManager* GetInstance()
		{
			static MaterialManager s_Instance;
			return &s_Instance;
		}

	protected:

	protected:
		std::unordered_map<uint32_t, std::shared_ptr<Material>> m_Map_Materials;
		std::vector<Material*> m_Materials;

		BufferManager* m_BufferManager;

		//
		//Friend Classes
		//
		friend class ModelManager;
	};

}


#define TE_INSTANCE_MATERIALMANAGER TruthEngine::MaterialManager::GetInstance()