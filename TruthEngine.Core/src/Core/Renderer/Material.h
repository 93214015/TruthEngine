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
				, uint8_t shaderProperties
				, float4 colorDiffuse
				, float3 fresnelR0
				, float shininess
				, uint32_t diffuseMapIndex
				, uint32_t normalMapIndex
				, uint32_t displacementMapIndex
			, int32_t extraDepthBias
			, float extraSlopeScaledDepthBias
			, float extraDepthBiasClamp);

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

		private:


		private:
			uint32_t m_ID;
			RendererStateSet m_RendererStates = InitRenderStates();
			uint8_t m_ShaderProperties;
			

			float4 m_ColorDiffuse;
			float3 m_FresnelR0;
			float m_Shininess;

			/*MaterialTexture* m_MapDiffuse = nullptr;
			MaterialTexture* m_MapNormal = nullptr;
			MaterialTexture* m_MapDisplacement = nullptr;*/

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
