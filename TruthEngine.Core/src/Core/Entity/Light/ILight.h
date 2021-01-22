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
				, Range(100.0f)
			{
				Math::Normalize(Direction);
				//ZeroMemory(this, sizeof(LightData));
			}

			LightData(float4 diffuseColor, float4 ambientColor, float4 specularColor, float3 direction, float lightSize, float3 position, uint32_t castShadow, float range)
				: Diffuse(diffuseColor), Ambient(ambientColor), Specular(specularColor), Direction(direction), LightSize(lightSize), Position(position), CastShadow(castShadow), Range(range)
			{
				Math::Normalize(Direction);
			}

			float4 Diffuse;
			float4 Ambient;
			float4 Specular;

			float3 Direction;
			float	 LightSize;

			float3 Position;
			uint32_t      CastShadow;

			float Range;
			float3 pad;
		};


		struct DirectionalLightData : public LightData
		{
			using LightData::LightData;
		};


		struct SpotLightData : public LightData
		{

			float3 Attenuation;
			float Spot;

		};


		class ILight
		{

		public:

			ILight(
				uint32_t id
				,std::string_view name
				, LightData* lightData
				, TE_LIGHT_TYPE lightType);

			virtual ~ILight();


			ILight(ILight&&) noexcept;
			ILight& operator=(ILight&&) noexcept;



			//
			//Set Methods
			//
			void SetDiffuseColor(const float4& _diffuseColor) noexcept;

			void SetAmbientColor(const float4& _ambientColor) noexcept;

			void SetSpecularColor(const float4& _specularColor)noexcept;

			void SetDirection(const float3& _direction) noexcept;

			void SetDirection(const float x, const float y, const float z) noexcept;

			void SetPosition(const float3& _position) noexcept;

			void SetPosition(const float x, const float y, const float z) noexcept;

			void SetCastShadow(const bool _castshadow) noexcept;

			void SetRange(const float _range) noexcept;


			//
			//Get Methods
			//

			inline uint32_t GetID() const noexcept
			{
				return m_ID;
			}
			inline const std::string& GetName()const noexcept
			{
				return m_Name;
			}
			inline float3 GetPosition()const
			{
				return m_LightData->Position;
			}

			inline float3 GetDirection()const
			{
				return m_LightData->Direction;
			}

			inline float4 GetDiffuseColor()const
			{
				return m_LightData->Diffuse;
			}

			inline float4 GetAmbientColor() const 
			{
				return m_LightData->Ambient;
			}

			inline float4 GetSpecularColor() const 
			{
				return m_LightData->Specular;
			}

			virtual LightData& GetLightData() const;

			inline bool GetCastShadow()const noexcept
			{
				return static_cast<bool>(m_LightData->CastShadow);;
			}

			inline TE_LIGHT_TYPE GetLightType()const noexcept
			{
				return m_LightType;
			}

			inline float GetRange()const noexcept
			{
				return m_LightData->Range;
			}

			//
			//abstract functions
			//

		protected:


		protected:

			std::string m_Name;

			uint32_t m_ID;

			LightData* m_LightData = nullptr;

			bool m_Disabled;

			bool m_ShadowDynamicObjects;

			TE_LIGHT_TYPE m_LightType;

			std::function<void(const float&)> m_Func_Update = [](const float& dt) {return; };
		};
	}
}

