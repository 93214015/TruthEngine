#include "pch.h"
#include "SystemUpdateTransform.h"

#include "Core/Application.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Components.h"


namespace TruthEngine
{
	void SystemUpdateTransform::OnUpdate(Scene* _Scene, double DeltaSecond)
	{
		/*for (auto _EntityHandler : _Scene->ViewEntities<UpdatedComponent>())
		{
			UpdatedComponent& _UpdatedComponent = _Scene->GetComponent<UpdatedComponent>(_EntityHandler);
			if (_UpdatedComponent.m_Updated == true)
			{
				const XMVector _XMTranslation = Math::ToXM(_Scene->GetComponent<TranslationComponent>(_EntityHandler).Translation);
				const XMVector _XMQuaternion = Math::ToXM(_Scene->GetComponent<RotationComponent>(_EntityHandler).Quaterion);

				const XMMatrix _XMTransform = Math::XMTransformMatrix(Math::XMIdentityScale, _XMQuaternion, _XMTranslation);
				float4x4A& _Transform = _Scene->GetComponent<TransformComponent>(_EntityHandler).GetTransform();
				_Transform = Math::FromXMA(_XMTransform);

				_UpdatedComponent.m_Updated = false;
			}
		}*/
	}
}