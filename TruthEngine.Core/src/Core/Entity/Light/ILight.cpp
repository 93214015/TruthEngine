#include "pch.h"
#include "ILight.h"

namespace TruthEngine::Core
{
	ILight::ILight(
		const std::string_view name
		, LightData* lightData
	, TE_LIGHT_TYPE lightType)
		: m_Name(name)
		, m_LightData(lightData)
		, m_Disabled(false)
		, m_ShadowDynamicObjects(false)
		, m_LightType(lightType)
	{
	}

	ILight::~ILight() = default;

	ILight::ILight(ILight&&)noexcept = default;

	ILight& ILight::operator=(ILight&&)noexcept = default;

	constexpr float4x4 ILight::m_ProjToUV = float4x4(0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);


	

	LightData& ILight::GetLightData() const
	{
		return *m_LightData;
	}


	void ILight::UpdateShadowTransformMatrix()
	{
		const auto viewProj = XMLoadFloat4x4(&m_LightData->ViewProj);
		const auto projToUV = XMLoadFloat4x4(&m_ProjToUV);

		const auto shadowTransform = XMMatrixMultiply(viewProj, projToUV);
		XMStoreFloat4x4(&m_LightData->ShadowTransform, shadowTransform);
	}

}
