#pragma once

enum class TE_LIGHT_TYPE
{
	Directional,
	Spot,
	Point
};

namespace TruthEngine
{


	struct DirectionalLightData
	{
		DirectionalLightData(const float3& _Position, float _LightSize, const float3& _Strength
			, float _StrengthMultiplier, const float3& _Direction, bool _CastShadow)
			: Position(_Position), LightSize(_LightSize)
			, Strength(_Strength), StrengthMultipier(_StrengthMultiplier)
			, Direction(_Direction), CastShadow(static_cast<uint32_t>(_CastShadow))
		{
			Math::Normalize(Direction);
		}
		float3 Position;
		float LightSize;

		float3 Strength;
		float StrengthMultipier;

		float3 Direction;
		uint32_t CastShadow;


	};


	struct SpotLightData
	{
		SpotLightData(const float3& _Position, float _LightSize,  const float3& _Strength
			, float _StrengthMultiplier, const float3& _Direction, bool _CastShadow
			, float _FalloffStart, float _FalloffEnd, float _SpotInnerConeCos, float _SpotOuterConeCos)
			: Position(_Position), LightSize(_LightSize)
			, Strength(_Strength), StrengthMultiplier(_StrengthMultiplier)
			, Direction(_Direction), CastShadow(static_cast<uint32_t>(_CastShadow))
			, FalloffStart(_FalloffStart), FalloffEnd(_FalloffEnd)
			, SpotInnerConeCos(_SpotInnerConeCos), SpotOuterConeCos(_SpotOuterConeCos)
		{
			Math::Normalize(Direction);
		}

		float4x4A ShadowTransform = IdentityMatrix;

		float3 Position;
		float LightSize;

		float3 Strength;
		float StrengthMultiplier = 0.0f;

		float3 Direction;
		uint32_t CastShadow;


		float FalloffStart;
		float FalloffEnd;
		float SpotInnerConeCos;
		float SpotOuterConeCos;
	};

	struct PointLightData
	{
		PointLightData(const float3& _Position, float _LightSize, const float3& _Strength, float _StrengthMultiplier, bool _CastShadow, float _AttenuationConstant, float _AttenuationLinear, float _AttenuationQuadrant)
			: Position(_Position), LightSize(_LightSize), Strength(_Strength), StrengthMultiplier(_StrengthMultiplier), CastShadow(static_cast<uint32_t>(_CastShadow)), AttenuationConstant(_AttenuationConstant), AttenuationLinear(_AttenuationLinear), AttenuationQuadrant(_AttenuationQuadrant)
		{}

		float3 Position;
		float LightSize;

		float3 Strength;
		float StrengthMultiplier = 0.0f;

		uint32_t CastShadow;
		float AttenuationConstant;
		float AttenuationLinear;
		float AttenuationQuadrant;
	};


	class ILight
	{

	public:

		ILight(
			uint32_t id
			, std::string_view name
			, TE_LIGHT_TYPE lightType);

		virtual ~ILight() = default;

		ILight(ILight&&) noexcept = default;
		ILight& operator=(ILight&&) noexcept = default;


		//
		//Set Methods
		//
		virtual void SetStrength(const float3& _Strength) noexcept = 0;

		virtual void SetStrengthMultiplier(float _StrengthMultiplier) noexcept = 0;

		virtual void SetCastShadow(const bool _castshadow) noexcept = 0;

		virtual void SetView(const float3& _Position, const float3& _NDirection, const float3& _NUp, const float3& _NRight) noexcept = 0;

		virtual void SetDirection(const float3& _NDirection, const float3& _NUp, const float3& _NRight) noexcept = 0;

		virtual void SetPosition(const float3& _Position) noexcept = 0;

		virtual void SetPosition(const float x, const float y, const float z) noexcept = 0;

		virtual void SetLightSize(float _LightSize) noexcept = 0;

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

		inline TE_LIGHT_TYPE GetLightType()const noexcept
		{
			return m_LightType;
		}


		virtual const float3& GetPosition() const noexcept = 0;

		virtual const float3& GetDirection() const noexcept = 0;

		virtual const float3& GetStrength() const noexcept = 0;

		virtual float GetStrengthMultiplier() const noexcept = 0;

		virtual bool GetCastShadow()const noexcept = 0;


	protected:
		void InvokeEventUpdateLight();

	protected:

		std::string m_Name;

		uint32_t m_ID;

		bool m_Disabled = false;

		bool m_ShadowDynamicObjects = false;

		TE_LIGHT_TYPE m_LightType;

		std::function<void(const float&)> m_Func_Update = [](const float& dt) {return; };
	};
}

