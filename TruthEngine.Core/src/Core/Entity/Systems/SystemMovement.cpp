#include "pch.h"
#include "SystemMovement.h"

#include "Core/Application.h"
#include "Core/Entity/Components.h"
#include "Core/Entity/Scene.h"

namespace TruthEngine
{
	void SystemMovement::OnUpdate(Scene* _Scene, double DeltaSecond)
	{

		for (auto _EntityHandler : _Scene->ViewEntities<MovementComponent>())
		{
			auto& _MovementComponent = _Scene->GetComponent<MovementComponent>(_EntityHandler);
			const XMVector _XMMovement = Math::ToXM(_MovementComponent.MovementVector);
			auto& _Transform = _Scene->GetComponent<TransformComponent>(_EntityHandler).GetTransform();
			XMMatrix _XMTransform = Math::ToXM(_Transform);

			if (_MovementComponent.IsAbsolutePostion)
			{
				_XMTransform.r[3] = Math::XMSelect(_XMMovement, XMVectorOne, Math::XMSelectVector(0, 0, 0, 1));
				_MovementComponent.IsAbsolutePostion = false;
			}
			else
			{
				_XMTransform.r[3] = Math::XMAdd(_XMTransform.r[3], _XMMovement);
			}

			_MovementComponent.MovementVector = float3A(0.0f, 0.0f, 0.0f);

			_Transform = Math::FromXMA(_XMTransform);

			_Scene->AddOrReplaceComponent<UpdatedComponent>(_EntityHandler, true);			
		}
	}
}