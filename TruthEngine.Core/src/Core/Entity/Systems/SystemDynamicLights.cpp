#include "pch.h"
#include "SystemDynamicLights.h"

#include "Core/Entity/Scene.h"
#include "Core/Entity/Components/DynamicLightComponent.h"
#include "Core/Entity/Components/LightComponent.h"
#include "Core/Entity/Components/TransformComponent.h"
#include "Core/Entity/Components/MovementComponent.h"
#include "Core/Entity/Components/UpdatedComponent.h"

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine
{
	void SystemDynamicLights::OnUpdate(Scene* _Scene, double DeltaSecond)
	{
		// Apply Transform to Light Object

		{
			auto _DynamicLightEntities = _Scene->ViewEntities<LightComponent, UpdatedComponent>();

			for (auto _Entity : _DynamicLightEntities)
			{
				UpdatedComponent& _UpdatedComponent = _Scene->GetComponent<UpdatedComponent>(_Entity);

				if (_UpdatedComponent.m_Updated)
				{
					XMMatrix _XMTransform = Math::ToXM(_Scene->GetComponent<TransformComponent>(_Entity).GetTransform());
					//XMVector _XMLightPosition = Math::ToXM(_Light->GetPosition());

					XMVector _XMLightPosition = Math::XMTransformVector4(XMVectorOrigin, _XMTransform);
					XMVector _XMLightDirection = Math::XMTransformVector4(Math::_XMVectorRow3, _XMTransform);

					ILight* _Light = _Scene->GetComponent<LightComponent>(_Entity).GetLight();
					_Light->SetPosition(Math::FromXM3(_XMLightPosition));
					_Light->SetDirection(Math::FromXM3(_XMLightDirection), float3(0.0f, 1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f));

					_UpdatedComponent.m_Updated = false;
				}
			}
		}

	}
}