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
		, const float3& strength
		, const float3& direction
		, const float3& position
		, const float lightSize
		, const int castShadow
		, const float4& cascadesCoveringDepth
		, CameraCascadedFrustumBase* cascadedCamera)
		: ILight(id, name, TE_LIGHT_TYPE::Directional)
		, m_DLightData(strength, lightSize, direction, castShadow, position)
		, m_Camera(cascadedCamera), m_CascadesDepth(cascadesCoveringDepth)
	{
	}

	void LightDirectional::SetStrength(const float3& _Strength) noexcept
	{
		m_DLightData.Strength = _Strength;

		InvokeEventUpdateLight();
	}

	void LightDirectional::SetCastShadow(const bool _castshadow) noexcept
	{
		m_DLightData.CastShadow = static_cast<uint32_t>(_castshadow);

		InvokeEventUpdateLight();
	}

	void LightDirectional::SetView(const float3& _Position, const float3& _Direction, const float3& _Up, const float3& _Right) noexcept
	{
		m_DLightData.Direction = _Direction;

		m_DLightData.Position = _Position;

		if (m_Camera)
			m_Camera->SetViewMatrix(_Position, _Direction, _Up, _Right);

		InvokeEventUpdateLight();
	}

	void LightDirectional::SetDirection(const float3& _Direction, const float3& _Up, const float3& _Right) noexcept
	{
		m_DLightData.Direction = _Direction;

		if (m_Camera)
			m_Camera->SetLook(_Direction, _Up, _Right);

		InvokeEventUpdateLight();
	}

	void LightDirectional::SetPosition(const float3& _position) noexcept
	{
		m_DLightData.Position = _position;

		if (m_Camera)
			m_Camera->SetPosition(m_DLightData.Position);

		InvokeEventUpdateLight();
	}

	void LightDirectional::SetPosition(const float x, const float y, const float z) noexcept
	{
		m_DLightData.Position = float3{ x, y, z };

		if (m_Camera)
			m_Camera->SetPosition(m_DLightData.Position);

		InvokeEventUpdateLight();
	}

	void LightDirectional::SetLightSize(float _LightSize) noexcept
	{
		m_DLightData.LightSize = _LightSize;

		InvokeEventUpdateLight();
	}

	const float3& LightDirectional::GetPosition() const noexcept
	{
		return m_DLightData.Position;
	}

	const float3& LightDirectional::GetDirection() const noexcept
	{
		return m_DLightData.Direction;
	}

	const float3& LightDirectional::GetStrength() const noexcept
	{
		return m_DLightData.Strength;
	}

	bool LightDirectional::GetCastShadow() const noexcept
	{
		return static_cast<bool>(m_DLightData.CastShadow);
	}

}