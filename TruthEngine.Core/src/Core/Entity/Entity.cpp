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

	void Entity::SetTransform(const float4x4A& _Transform) const
	{
		
	}

}