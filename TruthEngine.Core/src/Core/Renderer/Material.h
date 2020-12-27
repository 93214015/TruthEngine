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
				, float4 colorAmbient
				, float4 colorSpecular
				, MaterialTexture* diffuseMap
				, MaterialTexture* normalMap
				, MaterialTexture* displacementMap
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

			inline float4 GetColorAmbient() const noexcept
			{
				return m_ColorAmbient;
			}

			inline float4 GetColorSpecular() const noexcept
			{
				return m_ColorSpecular;
			}

		private:


		private:
			uint32_t m_ID;
			RendererStateSet m_RendererStates = InitRenderStates();
			uint8_t m_ShaderProperties;
			

			float4 m_ColorDiffuse;
			float4 m_ColorSpecular;
			float4 m_ColorAmbient;

			MaterialTexture* m_MapDiffuse = nullptr;
			MaterialTexture* m_MapNormal = nullptr;
			MaterialTexture* m_MapDisplacement = nullptr;

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
