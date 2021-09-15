#pragma once


namespace TruthEngine
{
		class Material
		{

		public:
			Material(
				uint32_t ID
				, RendererStateSet states
				, const float4& colorDiffuse
				, float _roughness
				, float _metallic
				, float _ambientOcclusion
				, float _emission
				//, const float3& fresnelR0
				//, float shininess
				, const float2& uvScale
				, const float2& uvTranslate
				, uint32_t diffuseMapIndex
				, uint32_t normalMapIndex
				, uint32_t displacementMapIndex
				, uint32_t specularMapIndex
				, uint32_t roughnessMapIndex
				, uint32_t metallicMapIndex
				, uint32_t ambientOcclusionMapIndex
				, int32_t extraDepthBias
				, float extraSlopeScaledDepthBias
				, float extraDepthBiasClamp
				, TE_IDX_MESH_TYPE meshType);

			//
			//////Get Methods
			//
			RendererStateSet GetRendererStates() const noexcept;

			inline uint32_t GetID() const noexcept
			{
				return m_ID;
			}

			inline const float4& GetColorDiffuse() const noexcept
			{
				return m_ColorDiffuse;
			}

			inline float GetMetallic() const noexcept
			{
				return m_Metallic;
			}

			inline float GetRoughness() const noexcept
			{
				return m_Roughness;
			}

			inline float GetAmbientOcclusion() const noexcept
			{
				return m_AmbientOccclusion;
			}

			inline float GetEmission() const noexcept
			{
				return m_Emission;
			}

			/*inline const float3& GetFresnelR0() const noexcept
			{
				return m_FresnelR0;
			}

			inline float GetShininess() const noexcept
			{
				return m_Shininess;
			}*/

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

			inline uint32_t GetMapIndexSpecular() const noexcept
			{
				return m_MapIndexSpecular;
			}

			inline uint32_t GetMapIndexRoughness() const noexcept
			{
				return m_MapIndexRoughness;
			}

			inline uint32_t GetMapIndexMetallic() const noexcept
			{
				return m_MapIndexMetallic;
			}

			inline uint32_t GetMapIndexAmbientOcclusion() const noexcept
			{
				return m_MapIndexAmbientOcclusion;
			}

			inline TE_IDX_MESH_TYPE GetMeshType() const noexcept
			{
				return m_MeshType;
			}

			TE_RENDERER_STATE_SHADING_MODEL GetShadingModel() const noexcept;


			//
			//////Set Methods
			//
			void InvokeEventChangeMaterial();

			void SetShadingModel(TE_RENDERER_STATE_SHADING_MODEL _ShadingModel);		

			inline void SetColorDiffuse(const float4& color)
			{
				m_ColorDiffuse = color;
				InvokeEventChangeMaterial();
			}

			inline void SetMetallic(float _Metallic)
			{
				m_Metallic = _Metallic;
				InvokeEventChangeMaterial();
			}

			inline void SetRoughness(float _Roughness)
			{
				m_Roughness = _Roughness;
				InvokeEventChangeMaterial();
			}

			inline void SetAmbientOcclusion(float _AO)
			{
				m_AmbientOccclusion = _AO;
				InvokeEventChangeMaterial();
			}

			inline void SetEmission(float _emission)
			{
				m_Emission = _emission;
				InvokeEventChangeMaterial();
			}

			/*inline void SetFresnelR0(const float3& r0)
			{
				m_FresnelR0 = r0;
				InvokeEventChangeMaterial();
			}

			inline void SetShininess(const float shininess)
			{
				m_Shininess = shininess;
				InvokeEventChangeMaterial();
			}*/

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

			void SetMapIndexSpecular(uint32_t index);

			void SetMapIndexRoughness(uint32_t index);

			void SetMapIndexMetallic(uint32_t index);

			void SetMapIndexAmbientOcclusion(uint32_t index);
			


		private:


		private:
			uint32_t m_ID;
			RendererStateSet m_RendererStates = InitRenderStates();

			TE_IDX_MESH_TYPE m_MeshType;

			float4 m_ColorDiffuse;

			float m_Roughness;
			float m_Metallic;
			float m_AmbientOccclusion;
			float m_Emission = 0.0f;
			//float3 m_FresnelR0;
			//float m_Shininess = 0.0f;

			float2 m_UVScale = { 1.0f, 1.0f };
			float2 m_UVTranslate = { .0f, .0f };

			uint32_t m_MapIndexDiffuse = -1;
			uint32_t m_MapIndexNormal = -1;
			uint32_t m_MapIndexDisplacement = -1;
			uint32_t m_MapIndexSpecular = -1;
			uint32_t m_MapIndexRoughness = -1;
			uint32_t m_MapIndexMetallic = -1;
			uint32_t m_MapIndexAmbientOcclusion = -1;


			int32_t m_ExtraDepthBias = 0;
			float m_ExtraSlopeScaledDepthBias = 0.0f;
			float m_ExtraDepthBiasClamp = 0.0f;

			//
			//Friend Classes
			//
			friend class MaterialManager;

		};
}
