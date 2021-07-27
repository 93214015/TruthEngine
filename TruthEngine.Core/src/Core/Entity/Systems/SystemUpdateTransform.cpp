#include "pch.h"
#include "SystemUpdateTransform.h"

#include "Core/Application.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Components.h"


namespace TruthEngine
{
	SystemUpdateTransform::SystemUpdateTransform()
		: System(Application::GetApplication()->GetActiveScene())
	{}

	void SystemUpdateTransform::OnUpdate(float DeltaTime)
	{
		for (auto _EntityHandler : m_Scene->ViewEntities<UpdatedComponent>())
		{
			UpdatedComponent& _UpdatedComponent = m_Scene->GetComponent<UpdatedComponent>(_EntityHandler);
			if (_UpdatedComponent.m_Updated == true)
			{
				const XMVector _XMTranslation = Math::ToXM(m_Scene->GetComponent<TranslationComponent>(_EntityHandler).Translation);
				const XMVector _XMQuaternion = Math::ToXM(m_Scene->GetComponent<RotationComponent>(_EntityHandler).Quaterion);

				const XMMatrix _XMTransform = Math::XMTransformMatrix(Math::XMIdentityScale, _XMQuaternion, _XMTranslation);
				float4x4A& _Transform = m_Scene->GetComponent<TransformComponent>(_EntityHandler).GetTransform();
				_Transform = Math::FromXMA(_XMTransform);

				_UpdatedComponent.m_Updated = false;
			}
		}
	}
}