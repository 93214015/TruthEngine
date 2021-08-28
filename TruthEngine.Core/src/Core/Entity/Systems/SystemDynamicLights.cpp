#include "pch.h"
#include "SystemDynamicLights.h"

#include "Core/Entity/Scene.h"
#include "Core/Entity/Components/DynamicLightComponent.h"
#include "Core/Entity/Components/LightComponent.h"
#include "Core/Entity/Components/TransformComponent.h"

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine
{
	void SystemDynamicLights::OnUpdate(Scene* _Scene, double DeltaSecond)
	{

		//Apply Movements to light transform

		{
			auto MovementEntities = _Scene->ViewEntities<DynamicLightMovementComponent>();

			for (auto _Entity : MovementEntities)
			{
				auto& _MovementComponent = _Scene->GetComponent<DynamicLightMovementComponent>(_Entity);
				XMVector _XMMovement = Math::ToXM(_MovementComponent.Movement);

				XMMatrix _XMTransform = Math::ToXM(_Scene->GetComponent<TransformComponent>(_Entity).GetTransform());

				if (_MovementComponent.IsPosition)
				{
					_XMTransform.r[3] = _XMMovement;
					_MovementComponent.IsPosition = false;
				}
				else
				{
					_XMTransform.r[3] = Math::XMAdd(_XMTransform.r[3], _XMMovement);
				}

				_MovementComponent.Movement = float3A(0.0f, 0.0f, 0.0f);
			}
		}

		{

			/*auto RotationEntities = _Scene->ViewEntities<DynamicLightRotationComponent>();

			for (auto _Entity : RotationEntities)
			{
				ILight* _Light = _Scene->GetComponent<LightComponent>(_Entity).GetLight();

				float3A _Direction;

				auto& _RotationComponent = _Scene->GetComponent<DynamicLightRotationComponent>(_Entity);
				float4A& _RotationQuaternion = _RotationComponent.RotationQuaternion;

				if (_RotationComponent.IsDirection)
				{
					_Direction = float3A(_RotationQuaternion.x, _RotationQuaternion.y, _RotationQuaternion.z);
					_RotationComponent.IsDirection = false;
				}
				else
				{
					_Direction = _Light->GetDirection();
					Math::Rotate(_Direction, _RotationQuaternion);
				}

				_RotationQuaternion = Math::IdentityQuaternion;

				Math::Normalize(_Direction);

				_Light->SetDirection(_Direction, float3(0.0f, 1.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
			}*/
		}

		// Apply Transform to Light Object

		{
			auto _DynamicLightEntities = _Scene->ViewEntities<DynamicLightComponent>();

			for (auto _Entity : _DynamicLightEntities)
			{
				ILight* _Light = _Scene->GetComponent<LightComponent>(_Entity).GetLight();
				auto _XMTransform = Math::ToXM(_Scene->GetComponent<TransformComponent>(_Entity).GetTransform());
				auto _XMLightPosition = Math::ToXM(_Light->GetPosition());

				_XMLightPosition = Math::XMTransformVector3Point(_XMLightPosition, _XMTransform);

				_Light->SetPosition(Math::FromXM3(_XMLightPosition));
			}
		}

	}
}