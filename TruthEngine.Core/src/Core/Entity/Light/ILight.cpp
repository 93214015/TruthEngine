#include "pch.h"
#include "ILight.h"

#include "Core/Application.h"
#include "Core/Event/EventEntity.h"


namespace TruthEngine::Core
{
	ILight::ILight(
		uint32_t id
		,std::string_view name
		, LightData* lightData
	, TE_LIGHT_TYPE lightType)
		: m_ID(id)
		, m_Name(name)
		, m_LightData(lightData)
		, m_Disabled(false)
		, m_ShadowDynamicObjects(false)
		, m_LightType(lightType)
	{
	}

	ILight::~ILight() = default;

	ILight::ILight(ILight&&)noexcept = default;

	ILight& ILight::operator=(ILight&&)noexcept = default;


	

	LightData& ILight::GetLightData() const
	{
		return *m_LightData;
	}

	void ILight::SetDiffuseColor(const float4& _diffuseColor) noexcept
	{
		m_LightData->Diffuse = _diffuseColor;

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetAmbientColor(const float4& _ambientColor) noexcept
	{
		m_LightData->Ambient = _ambientColor;

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetSpecularColor(const float4& _specularColor) noexcept
	{
		m_LightData->Specular = _specularColor;

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetDirection(const float3& _direction) noexcept
	{
		m_LightData->Direction = _direction;

		Math::NormalizeEst(m_LightData->Direction);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetDirection(const float x, const float y, const float z) noexcept
	{
		m_LightData->Direction = float3{ x, y, z };

		Math::NormalizeEst(m_LightData->Direction);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetPosition(const float3& _position) noexcept
	{
		m_LightData->Position = _position;

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetPosition(const float x, const float y, const float z) noexcept
	{
		m_LightData->Position = float3{ x, y, z };

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetCastShadow(const bool _castshadow) noexcept
	{
		m_LightData->CastShadow = _castshadow;

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void ILight::SetRange(const float _range) noexcept
	{
		m_LightData->Range = _range;

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

}
