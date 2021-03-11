#include "pch.h"
#include "LightDirectional.h"
#include "Core/Event/EventEntity.h"
#include "Core/Application.h"
#include "Core/Entity/Camera/CameraCascadedFrustum.h"


namespace TruthEngine
{
	LightDirectional::LightDirectional(
		uint32_t id
		, std::string_view name
		, const float4 diffusecolor
		, const float4 ambientColor
		, const float4 specularColor
		, const float3 direction
		, const float3 position
		, const float lightSize
		, const int castShadow
		, const float range
		, const float4& cascadesCoveringDepth
		, CameraCascadedFrustumBase* cascadedCamera)
		: ILight(id, name, &m_DLightData, TE_LIGHT_TYPE::Directional)
		, m_DLightData(diffusecolor, ambientColor, specularColor, direction, lightSize, position, castShadow, range)
		, m_Camera(cascadedCamera), m_CascadesDepth(cascadesCoveringDepth)
	{
	}

	void LightDirectional::SetDirection(const float3& _direction) noexcept
	{
		m_DLightData.Direction = _direction;

		Math::NormalizeEst(m_DLightData.Direction);

		if (m_Camera)
			m_Camera->SetLook(m_DLightData.Direction);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightDirectional::SetDirection(const float x, const float y, const float z) noexcept
	{
		m_DLightData.Direction = float3{ x, y, z };

		Math::NormalizeEst(m_DLightData.Direction);

		if (m_Camera)
			m_Camera->SetLook(m_DLightData.Direction);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightDirectional::SetPosition(const float3& _position) noexcept
	{
		m_DLightData.Position = _position;

		if (m_Camera)
			m_Camera->SetPosition(m_DLightData.Position);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void LightDirectional::SetPosition(const float x, const float y, const float z) noexcept
	{
		m_DLightData.Position = float3{ x, y, z };

		if (m_Camera)
			m_Camera->SetPosition(m_DLightData.Position);

		EventEntityUpdateLight event(this);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

}