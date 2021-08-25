#pragma once

namespace TruthEngine
{
	struct DynamicLightMovementComponent
	{
		DynamicLightMovementComponent() = default;
		DynamicLightMovementComponent(bool _IsPosition, const float3A& _Movement)
			: IsPosition(_IsPosition), Movement(_Movement)
		{}

		bool IsPosition = false;
		float3A Movement = float3A(0.0f, 0.0f, 0.0f);
	};

	struct DynamicLightRotationComponent
	{
		DynamicLightRotationComponent() = default;
		DynamicLightRotationComponent(const float4A& _RotationQuaternion)
			: RotationQuaternion(_RotationQuaternion)
		{}

		bool IsDirection = false;
		float4A RotationQuaternion = Math::IdentityQuaternion;
	};
}
