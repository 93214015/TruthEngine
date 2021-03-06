#pragma once
#include "ILight.h"

namespace TruthEngine
{
	class LightPoint final : public ILight
	{
	public:

		LightPoint(
			uint32_t _ID,
			const char* _Name,
			const float3& _Position,
			float _LightSize,
			const float3& _Strength,
			float _StrengthMultiplier,
			const bool _CastShadow,
			float _AttenuationStartRadius,
			float _AttenuationEndRadius
			);

		//
		//Set Methods
		//
		void SetStrength(const float3& _Strength) noexcept override;

		void SetStrengthMultiplier(float _StrengthMultiplier) noexcept override;

		void SetCastShadow(const bool _castshadow) noexcept override;

		void SetView(const float3& _Position, const float3& _NDirection, const float3& _NUp, const float3& _NRight) noexcept override
		{}

		void SetDirection(const float3& _NDirection, const float3& _NUp, const float3& _NRight) noexcept override
		{}

		void SetPosition(const float3& _Position) noexcept override;

		void SetPosition(const float x, const float y, const float z) noexcept override;

		void SetLightSize(float _LightSize) noexcept;

		/*void SetAttenuationConstant(float _AttenuationConstant) noexcept;
		void SetAttenuationLinear(float _AttenuationLinear) noexcept;
		void SetAttenuationQuadrant(float _AttenuationQuadrant) noexcept;*/

		void SetAttenuationStartRadius(float _AttenuationStartRadius) noexcept;

		void SetAttenuationEndRadius(float _AttenuationEndRadius) noexcept;

		//
		//Get Methods
		//
		
		const float3& GetPosition() const noexcept override;

		const float3& GetDirection() const noexcept override
		{
			return float3(0.0f, 0.0f, 0.0f);
		}

		const float3& GetStrength() const noexcept override;

		float GetStrengthMultiplier() const noexcept override;

		bool GetCastShadow()const noexcept override;

		//float GetAttuantionConstant()const noexcept;

		//float GetAttenuationLinear()const noexcept;

		//float GetAttenuationQuadrant()const noexcept;

		float GetAttenuationStartRadius() const noexcept;

		float GetAttenuationEndRadius() const noexcept;

		const PointLightData& GetLightData() const noexcept;

	private:
		PointLightData m_Data;

		friend class LightManager;
	};
}
