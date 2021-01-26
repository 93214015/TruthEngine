#pragma once
#include "Material.h"

namespace TruthEngine
{

	namespace Core
	{
		class BufferManager;
		class RendererCommand;

		class MaterialManager
		{
		public:
			MaterialManager();

			void Init(BufferManager* bufferManager);

			inline Material* GetMaterial(const uint32_t materialID)
			{
				return m_Map_Materials[materialID].get();
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
				, float4 colorDiffuse
				, float3 fresnelR0
				, float shininess
				, uint32_t diffuseMapIndex
				, uint32_t normalMapIndex
				, uint32_t displacementMapIndex
				, int32_t extraDepthBias
				, float extraSlopeScaledDepthBias
				, float extraDepthBiasClamp
				, TE_IDX_MESH_TYPE meshType);

			Material* AddMaterial(Material* material);

			void AddSampleMaterial();

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

}
