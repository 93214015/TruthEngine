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

			//
			//////Get Methods
			//
			inline RendererStateSet GetRendererStates() const noexcept
			{
				return m_RendererStates;
			}

			inline uint32_t GetID() const noexcept
			{
				return m_ID;
			}

			inline const float4& GetColorDiffuse() const noexcept
			{
				return m_ColorDiffuse;
			}

			inline const float3& GetFresnelR0() const noexcept
			{
				return m_FresnelR0;
			}

			inline float GetShininess() const noexcept
			{
				return m_Shininess;
			}

			inline const float2& GetUVScale() const noexcept
			{
				return m_UVScale;
			}

			inline const float2& GetUVTranslate() const noexcept
			{
				return m_UVTranslate;
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


			//
			//////Set Methods
			//
			void InvokeEventChangeMaterial();


			inline void SetColorDiffuse(const float4& color)
			{
				m_ColorDiffuse = color;
				InvokeEventChangeMaterial();
			}

			inline void SetFresnelR0(const float3& r0)
			{
				m_FresnelR0 = r0;
				InvokeEventChangeMaterial();
			}

			inline void SetShininess(const float shininess)
			{
				m_Shininess = shininess;
				InvokeEventChangeMaterial();
			}

			inline void SetUVScale(const float2& _uvScale)
			{
				m_UVScale = _uvScale;
				InvokeEventChangeMaterial();
			}

			inline void SetUVTranslate(const float2& _uvTranslate)
			{
				m_UVTranslate = _uvTranslate;
				InvokeEventChangeMaterial();
			}

			void SetMapIndexDiffuse(uint32_t index);

			void SetMapIndexNormal(uint32_t index);

			void SetMapIndexDisplacement(uint32_t index);
			


		private:


		private:
			uint32_t m_ID;
			RendererStateSet m_RendererStates = InitRenderStates();

			TE_IDX_MESH_TYPE m_MeshType;

			float4 m_ColorDiffuse;
			float3 m_FresnelR0;
			float m_Shininess;

			float2 m_UVScale = { 1.0f, 1.0f };
			float2 m_UVTranslate = { .0f, .0f };

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
