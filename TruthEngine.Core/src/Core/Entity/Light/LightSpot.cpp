#include "pch.h"
#include "LightSpot.h"

#include "Core/Event/EventEntity.h"
#include "Core/Application.h"

#include "Core/Entity/Camera/Camera.h"



namespace TruthEngine
{

	constexpr float4x4 m_ProjToUV = 
		float4x4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	LightSpot::LightSpot(uint32_t _ID, std::string_view _Name, const float3& _Strength, const float _LightSize, const float3& _Direction, const bool _CastShadow, const float3& _Position, const float _FalloffStart, const float _FalloffEnd, const float _InnerConeAngle, const float _OuterConeAngle)
		: ILight(_ID, _Name, TE_LIGHT_TYPE::Spot),
		m_Data(_Strength, _LightSize, _Direction, _CastShadow, _Position, _FalloffStart, _FalloffEnd, cosf(Math::DegreeToRadian(_OuterConeAngle)), 1 / (cosf(Math::DegreeToRadian(_InnerConeAngle)) - cosf(Math::DegreeToRadian(_OuterConeAngle))))
	{}

	void LightSpot::SetStrength(const float3& _Strength) noexcept
	{
		m_Data.Strength = _Strength;

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetCastShadow(const bool _Castshadow) noexcept
	{
		m_Data.CastShadow = static_cast<uint32_t>(_Castshadow);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetView(const float3& _Position, const float3& _Direction, const float3& _Up, const float3& _Right) noexcept
	{
		m_Data.Direction = _Direction;
		m_Data.Position = _Position;

		if (m_Camera)
		{
			m_Camera->SetViewMatrix(_Position, _Direction, _Up, _Right);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetDirection(const float3& _Direction, const float3& _Up, const float3& _Right) noexcept
	{
		m_Data.Direction = _Direction;

		if (m_Camera)
		{
			m_Camera->SetLook(_Direction, _Up, _Right);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetPosition(const float3& _Position) noexcept
	{
		m_Data.Position = _Position;

		if (m_Camera)
		{
			m_Camera->SetPosition(_Position);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetPosition(const float x, const float y, const float z) noexcept
	{
		m_Data.Position = float3{ x,y,z };

		if (m_Camera)
		{
			m_Camera->SetPosition(m_Data.Position);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetFalloffStart(float _FalloffStart)
	{
		m_Data.FalloffStart = _FalloffStart;


		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetFalloffEnd(float _FalloffEnd)
	{
		m_Data.FalloffEnd = _FalloffEnd;

		if (m_Camera)
		{
			m_Camera->SetZFarPlane(_FalloffEnd);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetInnerConeAngle(float _InnerConeAngleDegree)
	{
		m_Data.SpotOuterConeAngleRangeCosRcp = 1 / (cosf(Math::DegreeToRadian(_InnerConeAngleDegree)) - m_Data.SpotOuterConeCos);


		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightSpot::SetOuterConeAngle(float _OuterConeAngleDegree)
	{
		float _NewAngleRangeCos = m_Data.SpotOuterConeCos - cosf(Math::DegreeToRadian(_OuterConeAngleDegree));
		float _OuterConeAngleRadian = Math::DegreeToRadian(_OuterConeAngleDegree);
		m_Data.SpotOuterConeCos = cosf(_OuterConeAngleRadian);
		m_Data.SpotOuterConeAngleRangeCosRcp = 1 / ( (1 / m_Data.SpotOuterConeAngleRangeCosRcp) + _NewAngleRangeCos ) ;

		if (m_Camera)
		{
			m_Camera->SetFOVY(_OuterConeAngleRadian);

			m_Data.ShadowTransform = m_Camera->GetViewProj() * m_ProjToUV;
		}

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
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

	bool LightSpot::GetCastShadow() const noexcept
	{
		return static_cast<bool>(m_Data.CastShadow);
	}

	float LightSpot::GetInnerConeAngle() const noexcept
	{
		/*float _OuterAngle = acosf(m_Data.SpotOuterConeCos);
		float _AngleRange = (1 / m_Data.SpotOuterConeAngleRangeCosRcp);

		return _OuterAngle - _AngleRange;*/

		float _Angle = 1 / m_Data.SpotOuterConeAngleRangeCosRcp;

		_Angle += m_Data.SpotOuterConeCos;

		return (acosf(_Angle));
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