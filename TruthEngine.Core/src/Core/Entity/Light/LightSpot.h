#pragma once
#include "ILight.h"

namespace TruthEngine
{

	class Camera;

	class LightSpot : public ILight
	{
	public:
		LightSpot(
			uint32_t _ID,
			std::string_view _Name,
			const float3& _Position,
			float _LightSize,
			const float3& _Strength,
			float _StrengthMultiplier,
			const float3& _Direction,
			const bool _CastShadow,
			const float _FalloffStart,
			const float _FalloffEnd,
			const float _InnerConeAngle,
			const float _OuterConeAngle
		);

		//
		//Set Methods
		//
		virtual void SetStrength(const float3& _Strength) noexcept override;

		virtual void SetStrengthMultiplier(float _StrengthMultiplier) noexcept override;

		virtual void SetCastShadow(const bool _Castshadow) noexcept override;

		virtual void SetView(const float3& _Position, const float3& _Direction, const float3& _Up, const float3& _Right) noexcept override;
		
		virtual void SetDirection(const float3& _Direction, const float3& _Up, const float3& _Right) noexcept override;

		virtual void SetPosition(const float3& _Position) noexcept override;

		virtual void SetPosition(const float x, const float y, const float z) noexcept override;

		virtual void SetLightSize(float _LightSize) noexcept override;

		void SetFalloffStart(float _FalloffStart);

		void SetFalloffEnd(float _FalloffEnd);

		void SetInnerConeAngle(float _InnerConeAngleDegree);

		void SetOuterConeAngle(float _OuterConeAngleDegree);

		//
		//Get Methods
		//
		virtual const float3& GetPosition() const noexcept override;

		virtual const float3& GetDirection() const noexcept override;

		virtual const float3& GetStrength() const noexcept override;

		virtual float GetStrengthMultiplier() const noexcept override;

		virtual bool GetCastShadow()const noexcept;

		inline const SpotLightData& GetLightData() const noexcept
		{
			return m_Data;
		}

		inline float GetFalloffStart() const noexcept
		{
			return m_Data.FalloffStart;
		}

		inline float GetFalloffEnd() const noexcept
		{
			return m_Data.FalloffEnd;
		}

		float GetInnerConeAngle() const noexcept;

		float GetOuterConeAngle() const noexcept;

		Camera* GetCamera() const
		{
			return m_Camera;
		}

		
	private:
		void SetCamera(Camera* _Camera);

	private:
		SpotLightData m_Data;

		Camera* m_Camera;

		friend class LightManager;
	};
}

