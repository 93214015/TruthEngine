#pragma once

enum class TE_LIGHT_TYPE
{
	Directional,
	Spot,
	Point
};

namespace TruthEngine
{

	namespace Core
	{

		//
	//Light Data
	//
		struct LightData
		{
			LightData() : 
				Diffuse(float4{ 1.0f, 1.0f, 1.0f, 1.0f })
				, Ambient(0.2f, 0.2f, 0.2f, 1.0f )
				, Specular(0.4f, 0.4f, 0.4f, 1.0f)
				, Direction(1.0f, -1.0f, 1.0f)
				, LightSize(0.01f)
				, Position(0.0f, 30.0f, 0.0f)
				, CastShadow(false)
				, IndexShadowMapSRV(-1)
			{
				//ZeroMemory(this, sizeof(LightData));
			}

			LightData(float4 diffuseColor, float4 ambientColor, float4 specularColor, float3 direction, float lightSize, float3 position, int32_t castShadow)
				: Diffuse(diffuseColor), Ambient(ambientColor), Specular(specularColor), Direction(direction), LightSize(lightSize), Position(position), CastShadow(castShadow)
			{}

			float4x4 View;
			float4x4 ViewProj;
			float4x4 ShadowTransform;

			float4 PerpectiveValues;

			float4 Diffuse;
			float4 Ambient;
			float4 Specular;

			float3 Direction;
			float	 LightSize;

			float3 Position;
			float	 zNear;

			float2 DirInEyeScreen_Horz;
			float2 DirInEyeScreen_Vert;

			float	     zFar;
			int32_t      CastShadow;
			int32_t      IndexShadowMapSRV;
			float		 pad;
		};


		struct DirectionalLightData : public LightData
		{
			using LightData::LightData;

			float4 pad[12];
		};


		struct SpotLightData : public LightData
		{

			float3 Attenuation;
			float Range;

			float Spot;
			float3 pad;

			float4 pad2[10];
		};


		class ILight
		{

		public:

			ILight(const std::string_view name
				, LightData* lightData
				, TE_LIGHT_TYPE lightType);

			virtual ~ILight();


			ILight(ILight&&) noexcept;
			ILight& operator=(ILight&&) noexcept;


			virtual LightData& GetLightData() const;

			inline void UpdateDiffuseColor(const float4 _diffuseColor) const noexcept
			{
				m_LightData->Diffuse = _diffuseColor;
			}

			inline void UpdateAmbientColor(const float4 _ambientColor) const noexcept
			{
				m_LightData->Ambient = _ambientColor;
			}

			inline void UpdateSpecularColor(const float4 _specularColor) const noexcept
			{
				m_LightData->Specular = _specularColor;
			}

			inline void SetDirection(const float3 _direction) const noexcept
			{
				m_LightData->Direction = _direction;
			}

			inline void SetDirection(const float x, const float y, const float z) const noexcept
			{
				m_LightData->Direction = float3{ x, y, z };
			}

			inline void SetPosition(const float3 _position) const noexcept
			{
				m_LightData->Position = _position;
			}

			inline void SetPosition(const float x, const float y, const float z) const noexcept
			{
				m_LightData->Position = float3{ x, y, z };
			}

			inline void SetCastShadow(const bool _castshadow) const noexcept
			{
				m_LightData->CastShadow = _castshadow;
			}

			inline bool GetCastShadow()const noexcept
			{
				return static_cast<bool>(m_LightData->CastShadow);;
			}



			inline TE_LIGHT_TYPE GetLightType()const noexcept
			{
				return m_LightType;
			}

			//
			//abstract functions
			//

		protected:

			virtual void UpdateShadowTransformMatrix();


		protected:

			std::string m_Name;

			LightData* m_LightData = nullptr;

			bool m_Disabled;

			bool m_ShadowDynamicObjects;

			TE_LIGHT_TYPE m_LightType;

			std::function<void(const float&)> m_Func_Update = [](const float& dt) {return; };

			static const float4x4 m_ProjToUV;
		};
	}
}

