#include "pch.h"
#include "LightSpot.h"

#include "Core/Event/EventEntity.h"
#include "Core/Application.h"

#include "Core/Entity/Camera/Camera.h"



namespace TruthEngine
{

	constexpr float4x4A m_ProjToUV = 
		float4x4A(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	LightSpot::LightSpot(
		uint32_t _ID
		, std::string_view _Name
		, const float3& _Position
		, const float _LightSize
		, const float3& _Strength
		, float _StrengthMultiplier
		, const float3& _Direction
		, const bool _CastShadow
		, const float _FalloffStart
		, const float _FalloffEnd
		, const float _InnerConeAngle
		, const float _OuterConeAngle)
		: ILight(_ID, _Name, TE_LIGHT_TYPE::Spot),
		m_Data(_Position, _LightSize, _Strength, _StrengthMultiplier, _Direction, _CastShadow, _FalloffStart, _FalloffEnd, cosf(Math::DegreeToRadian(_InnerConeAngle)), cosf(Math::DegreeToRadian(_OuterConeAngle)))
	{}

	void LightSpot::SetStrength(const float3& _Strength) noexcept
	{
		m_Data.Strength = _Strength;

		InvokeEventUpdateLight();
	}

	void LightSpot::SetStrengthMultiplier(float _StrengthMultiplier) noexcept
	{
		m_Data.StrengthMultiplier = _StrengthMultiplier;

		InvokeEventUpdateLight();
	}

	void LightSpot::SetCastShadow(const bool _Castshadow) noexcept
	{
		m_Data.CastShadow = static_cast<uint32_t>(_Castshadow);

		InvokeEventUpdateLight();
	}

	void LightSpot::SetView(const float3& _Position, const float3& _Direction, const float3& _Up, const float3& _Right) noexcept
	{
		m_Data.Direction = _Direction;
		m_Data.Position = _Position;

		if (m_Camera)
		{
			m_Camera->SetView(_Position, _Direction, _Up, _Right);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		InvokeEventUpdateLight();
	}

	void LightSpot::SetDirection(const float3& _Direction, const float3& _Up, const float3& _Right) noexcept
	{
		m_Data.Direction = _Direction;

		if (m_Camera)
		{
			m_Camera->SetView(_Direction, _Up, _Right);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		InvokeEventUpdateLight();
	}

	void LightSpot::SetPosition(const float3& _Position) noexcept
	{
		m_Data.Position = _Position;

		if (m_Camera)
		{
			m_Camera->SetPosition(_Position);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		InvokeEventUpdateLight();
	}

	void LightSpot::SetPosition(const float x, const float y, const float z) noexcept
	{
		m_Data.Position = float3{ x,y,z };

		if (m_Camera)
		{
			m_Camera->SetPosition(m_Data.Position);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		InvokeEventUpdateLight();
	}

	void LightSpot::SetLightSize(float _LightSize) noexcept
	{
		m_Data.LightSize = _LightSize;

		InvokeEventUpdateLight();
	}

	void LightSpot::SetFalloffStart(float _FalloffStart)
	{
		m_Data.FalloffStart = _FalloffStart;


		InvokeEventUpdateLight();
	}

	void LightSpot::SetFalloffEnd(float _FalloffEnd)
	{
		m_Data.FalloffEnd = _FalloffEnd;

		if (m_Camera)
		{
			m_Camera->SetZFarPlane(_FalloffEnd);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		InvokeEventUpdateLight();
	}

	void LightSpot::SetInnerConeAngle(float _InnerConeAngleDegree)
	{
		m_Data.SpotInnerConeCos = cosf(Math::DegreeToRadian(_InnerConeAngleDegree));


		InvokeEventUpdateLight();
	}

	void LightSpot::SetOuterConeAngle(float _OuterConeAngleDegree)
	{
		m_Data.SpotOuterConeCos = cosf(Math::DegreeToRadian(_OuterConeAngleDegree));

		/*float _NewAngleRangeCos = m_Data.SpotOuterConeCos - cosf(Math::DegreeToRadian(_OuterConeAngleDegree));
		float _OuterConeAngleRadian = Math::DegreeToRadian(_OuterConeAngleDegree);
		m_Data.SpotOuterConeAngleRangeCosRcp = 1 / ( (1 / m_Data.SpotOuterConeAngleRangeCosRcp) + _NewAngleRangeCos ) ;

		if (m_Camera)
		{
			m_Camera->SetFOVY(_OuterConeAngleRadian);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}*/

		InvokeEventUpdateLight();
	}

	const float3& LightSpot::GetPosition() const noexcept
	{
		return m_Data.Position;
	}

	const float3& LightSpot::GetDirection() const noexcept
	{
		return m_Data.Direction;
	}

	const float3& LightSpot::GetStrength() const noexcept
	{
		return m_Data.Strength;
	}

	float LightSpot::GetStrengthMultiplier() const noexcept
	{
		return m_Data.StrengthMultiplier;
	}

	bool LightSpot::GetCastShadow() const noexcept
	{
		return static_cast<bool>(m_Data.CastShadow);
	}

	float LightSpot::GetInnerConeAngle() const noexcept
	{
		/*float _OuterAngle = acosf(m_Data.SpotOuterConeCos);
		float _AngleRange = (1 / m_Data.SpotOuterConeAngleRangeCosRcp);

		return _OuterAngle - _AngleRange;*/
		

		return (acosf(m_Data.SpotInnerConeCos));
	}

	float LightSpot::GetOuterConeAngle() const noexcept
	{
		return acosf(m_Data.SpotOuterConeCos);
	}


	void LightSpot::SetCamera(Camera* _Camera)
	{
		m_Camera = _Camera;

		m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
	}



}