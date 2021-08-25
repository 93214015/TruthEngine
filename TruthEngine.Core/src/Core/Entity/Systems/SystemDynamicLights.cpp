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
		{
			auto MovementEntities = _Scene->ViewEntities<DynamicLightMovementComponent>();

			for (auto _Entity : MovementEntities)
			{
				ILight* _Light = _Scene->GetComponent<LightComponent>(_Entity).GetLight();

				float3A _Position;

				auto& _MovementComponent = _Scene->GetComponent<DynamicLightMovementComponent>(_Entity);
				float3A& _Movement = _MovementComponent.Movement;

				if (_MovementComponent.IsPosition)
				{
					_Position = _Movement;
					_MovementComponent.IsPosition = false;
				}
				else
				{
					_Position = _Light->GetPosition();
					_Position += _Movement;
				}

				_Movement = float3A(0.0f, 0.0f, 0.0f);
				_Light->SetPosition(_Position);

				auto _Transform = _Scene->GetComponent<TransformComponent>(_Entity).GetTransform();

				_Transform._41 = _Position.x;
				_Transform._42 = _Position.y;
				_Transform._43 = _Position.z;

			}
		}

		{

			auto RotationEntities = _Scene->ViewEntities<DynamicLightRotationComponent>();

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

				Math::Normalize(_Direction);

				_Light->SetDirection(_Direction, float3(0.0f, 1.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
			}
		}


	}
}