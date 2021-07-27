#include "pch.h"
#include "Entity.h"

#include "Scene.h"

#include "Components.h"

using namespace DirectX;

namespace TruthEngine
{
	Entity::Entity()
		: m_EntityHandle(entt::null)
	{
	}



	Entity::Entity(entt::entity entityHandle)
		: m_EntityHandle(entityHandle)
	{}

	float4x4A Entity::GetTransformHierarchy()
	{
		return Application::GetActiveScene()->GetTransformHierarchy(*this);
	}


	/*float3 Entity::GetPosition() noexcept
	{
		const auto& _transform = m_Scene->GetTransformHierarchy(m_EntityHandle);
		float3 _worldPostition = { 0.0f, 0.0f, 0.0f };
		if (HasComponent<BoundingBoxComponent>())
		{
			const auto& _aabb = GetComponent<BoundingBoxComponent>().GetBoundingBox();

			_worldPostition.x += _aabb.Center.x;
			_worldPostition.y += _aabb.Center.y;
			_worldPostition.z += _aabb.Center.z;
		}

		auto xmPos = XMVector3TransformCoord(XMLoadFloat3(&_worldPostition), XMLoadFloat4x4(&_transform));
		XMStoreFloat3(&_worldPostition, xmPos);

		return  _worldPostition;
	}*/


	void Entity::Move(const float4A& _MovementVector) const
	{
		Scene* _Scene = Application::GetActiveScene();
		XMVector _XMTranslate = Math::ToXM(_Scene->GetComponent<TranslationComponent>(*this).Translation);
		XMVector _XMMovement = Math::ToXM(_MovementVector);
		_XMTranslate = Math::XMAdd(_XMTranslate, _XMMovement);
		_Scene->GetComponent<TranslationComponent>(*this).Translation = Math::FromXMA(_XMTranslate);
	}

	void Entity::Rotate(const float4A& _RotationQuaternion) const
	{
		Scene* _Scene = Application::GetActiveScene();

		float4A& _Quat = _Scene->GetComponent<RotationComponent>(*this).Quaterion;

		_Quat = Math::QuaternionMultiply(_Quat, _RotationQuaternion);
	}

}