#include "pch.h"
#include "SystemDynamicLights.h"

#include "Core/Entity/Scene.h"
#include "Core/Entity/Components/DynamicLightComponent.h"
#include "Core/Entity/Components/LightComponent.h"
#include "Core/Entity/Components/TransformComponent.h"
#include "Core/Entity/Components/MovementComponent.h"

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine
{
	void SystemDynamicLights::OnUpdate(Scene* _Scene, double DeltaSecond)
	{

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
			auto _DynamicLightEntities = _Scene->ViewEntities<LightComponent, MovementComponent>();

			for (auto _Entity : _DynamicLightEntities)
			{
				ILight* _Light = _Scene->GetComponent<LightComponent>(_Entity).GetLight();
				XMMatrix _XMTransform = Math::ToXM(_Scene->GetComponent<TransformComponent>(_Entity).GetTransform());
				XMVector _XMLightPosition = Math::ToXM(_Light->GetPosition());

				_XMLightPosition = Math::XMTransformVector3Point(_XMLightPosition, _XMTransform);

				_Light->SetPosition(Math::FromXM3(_XMLightPosition));
			}
		}

	}
}