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

			inline size_t GetMatrialOffset() const noexcept
			{
				return m_Map_Materials.size();
			}

			Material* AddMaterial(
				 RendererStateSet states
				, uint8_t shaderProperties
				, float4 colorDiffuse
				, float4 colorAmbient
				, float4 colorSpecular
				, MaterialTexture* diffuseMap
				, MaterialTexture* normalMap
				, MaterialTexture* displacementMap
				, int32_t extraDepthBias
				, float extraSlopeScaledDepthBias
				, float extraDepthBiasClamp);

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
