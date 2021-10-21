#include "pch.h"
#include "ILight.h"

#include "Core/Application.h"
#include "Core/Event/EventEntity.h"


namespace TruthEngine
{
	ILight::ILight(
		uint32_t id
		,std::string_view name
	, TE_LIGHT_TYPE lightType)
		: m_ID(id)
		, m_Name(name)
		, m_LightType(lightType)
	{
	}


	/*void ILight::SetDirection(const float3& _direction) noexcept
	{
		m_LightData->Direction = _direction;

		Math::NormalizeEst(m_LightData->Direction);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}*/


	void ILight::InvokeEventUpdateLight()
	{
		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

}
