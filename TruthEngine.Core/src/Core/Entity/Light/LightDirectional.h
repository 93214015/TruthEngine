#pragma once
#include "ILight.h"

#include "Core/Entity/Camera/CameraCascadedFrustum.h"

namespace TruthEngine
{
	class CameraCascadedFrustumBase;

	class LightDirectional : public ILight
	{
	public:
		LightDirectional(
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
			, const float4& cascadesDepth
			, CameraCascadedFrustumBase* cascadedCamera);

		inline DirectionalLightData& GetDirectionalLightData() noexcept {
			return m_DLightData;
		}

		inline CameraCascadedFrustumBase* GetCamera() const noexcept
		{
			return m_Camera;
		}

		inline const float4& GetCascadesConveringDepth() const noexcept
		{
			return m_CascadesDepth;
		}

		inline void SetCascadesDepth(const float4& _CascadeDepths)
		{
			m_CascadesDepth = _CascadeDepths;
			const float _depths[] = { _CascadeDepths.x, _CascadeDepths.y, _CascadeDepths.z, _CascadeDepths.w };
			if (m_Camera)
				m_Camera->SetCascadesConveringDepth(_depths);
		}

		virtual void SetDirection(const float3& _direction) noexcept override;

		virtual void SetDirection(const float x, const float y, const float z) noexcept override;

		virtual void SetPosition(const float3& _position) noexcept override;

		virtual void SetPosition(const float x, const float y, const float z) noexcept override;

		

	protected:

		CameraCascadedFrustumBase* m_Camera = nullptr;

		float4 m_CascadesDepth;

		DirectionalLightData m_DLightData;

	};
}
