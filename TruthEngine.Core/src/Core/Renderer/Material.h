#pragma once


namespace TruthEngine
{
	namespace Core
	{

		class MaterialTexture;

		class Material
		{

		public:
			Material(
				uint32_t ID
				, RendererStateSet states
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

			inline RendererStateSet GetRendererStates() const noexcept
			{
				return m_RendererStates;
			}

			inline uint32_t GetID() const noexcept
			{
				return m_ID;
			}

			inline float4 GetColorDiffuse() const noexcept
			{
				return m_ColorDiffuse;
			}

			inline float3 GetFresnelR0() const noexcept
			{
				return m_FresnelR0;
			}

			inline float GetShininess() const noexcept
			{
				return m_Shininess;
			}

			inline uint32_t GetMapIndexDiffuse()const noexcept
			{
				return m_MapIndexDiffuse;
			}

			inline uint32_t GetMapIndexNormal()const noexcept
			{
				return m_MapIndexNormal;
			}

			inline uint32_t GetMapIndexDisplacement()const noexcept
			{
				return m_MapIndexDisplacement;
			}

			inline TE_IDX_MESH_TYPE GetMeshType() const noexcept
			{
				return m_MeshType;
			}

		private:


		private:
			uint32_t m_ID;
			RendererStateSet m_RendererStates = InitRenderStates();

			TE_IDX_MESH_TYPE m_MeshType;

			float4 m_ColorDiffuse;
			float3 m_FresnelR0;
			float m_Shininess;

			uint32_t m_MapIndexDiffuse = -1;
			uint32_t m_MapIndexNormal = -1;
			uint32_t m_MapIndexDisplacement = -1;

			int32_t m_ExtraDepthBias = 0;
			float m_ExtraSlopeScaledDepthBias = 0.0f;
			float m_ExtraDepthBiasClamp = 0.0f;

			//
			//Friend Classes
			//
			friend class MaterialManager;

		};
	}
}
