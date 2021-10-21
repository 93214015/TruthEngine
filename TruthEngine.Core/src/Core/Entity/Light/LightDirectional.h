#pragma once
#include "ILight.h"

#include "Core/Entity/Camera/CameraCascadedFrustum.h"

namespace TruthEngine
{
	class CameraCascadedFrustumBase;

	class LightDirectional final : public ILight
	{
	public:
		LightDirectional(
			uint32_t id
			, std::string_view name
			, const float3& position
			, const float lightSize
			, const float3& strength
			, float strengthMultiplier
			, const float3& direction
			, const int castShadow
			, const float4& cascadesDepth
			, CameraCascadedFrustumBase* cascadedCamera);

		inline const DirectionalLightData& GetDirectionalLightData() const noexcept {
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

		//
		//Set Methods
		//

		virtual void SetStrength(const float3& _Strength) noexcept override;

		virtual void SetStrengthMultiplier(float _StrengthMultiplier) noexcept override;

		virtual void SetCastShadow(const bool _castshadow) noexcept override;

		virtual void SetView(const float3& _Position, const float3& _Direction, const float3& _Up, const float3& _Right) noexcept;

		virtual void SetDirection(const float3& _Direction, const float3& _Up, const float3& _Right) noexcept override;

		virtual void SetPosition(const float3& _position) noexcept override;

		virtual void SetPosition(const float x, const float y, const float z) noexcept override;

		virtual void SetLightSize(float _LightSize) noexcept override;

		//
		//Get Methods
		//

		virtual const float3& GetPosition() const noexcept override;

		virtual const float3& GetDirection() const noexcept override;

		virtual const float3& GetStrength() const noexcept override;

		virtual float GetStrengthMultiplier() const noexcept override;

		virtual bool GetCastShadow()const noexcept override;
		

	protected:

		CameraCascadedFrustumBase* m_Camera = nullptr;

		float4 m_CascadesDepth;

		DirectionalLightData m_DLightData;

	};
}
