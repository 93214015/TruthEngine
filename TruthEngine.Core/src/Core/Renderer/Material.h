#pragma once


namespace TruthEngine
{
	namespace Core
	{

		class MaterialTexture;

		class Material
		{

		public:
			inline RendererStateSet GetRendererStates() const noexcept
			{
				return m_RendererStates;
			}

			inline uint32_t GetID() const noexcept
			{
				return m_ID;
			}

		private:


		private:
			uint32_t m_ID;
			RendererStateSet m_RendererStates = InitRenderStates();
			uint8_t m_ShaderProperties;
			

			DirectX::XMFLOAT4 m_ColorDiffuse;
			DirectX::XMFLOAT4 m_ColorSpecular;
			DirectX::XMFLOAT3 m_ColorAmbient;

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
