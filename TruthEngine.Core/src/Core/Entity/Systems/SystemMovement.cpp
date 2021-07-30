#include "pch.h"
#include "SystemMovement.h"

#include "Core/Application.h"
#include "Core/Entity/Components.h"
#include "Core/Entity/Scene.h"

namespace TruthEngine
{
	SystemMovement::SystemMovement()
		: System(GetActiveScene())
	{}

	void SystemMovement::OnUpdate(float DeltaTime)
	{
		for (auto _EntityHandler : m_Scene->ViewEntities<MovementComponent>())
		{
			const XMVector _XMMovement = Math::ToXM(m_Scene->GetComponent<MovementComponent>(_EntityHandler).MovementVector);
			if (Math::XMNotEqual3(_XMMovement, Math::XMIdentityTranslate))
			{
				float4A& _Translate = m_Scene->GetComponent<TranslationComponent>(_EntityHandler).Translation;
				XMVector _XMTranslate = Math::ToXM(_Translate);
				_XMTranslate = Math::XMAdd(_XMTranslate, _XMMovement);
				_Translate = Math::FromXMA(_XMTranslate);

				m_Scene->AddOrReplaceComponent<UpdatedComponent>(_EntityHandler, true);
			}
		}
	}
}