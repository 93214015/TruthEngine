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
			const XMVector _XMMovement = Math::ToXM(_Scene->GetComponent<MovementComponent>(_EntityHandler).MovementVector);
			if (Math::XMNotEqual3(_XMMovement, Math::XMIdentityTranslate))
			{
				float4A& _Translate = _Scene->GetComponent<TranslationComponent>(_EntityHandler).Translation;
				XMVector _XMTranslate = Math::ToXM(_Translate);
				_XMTranslate = Math::XMAdd(_XMTranslate, _XMMovement);
				_Translate = Math::FromXMA(_XMTranslate);

				_Scene->AddOrReplaceComponent<UpdatedComponent>(_EntityHandler, true);
			}
		}
	}
}