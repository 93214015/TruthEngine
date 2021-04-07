#include "pch.h"
#include "PhysicsComponent.h"

using namespace DirectX;

namespace TruthEngine
{

	PhysicsStaticComponent::PhysicsStaticComponent(physx::PxRigidStatic* actor)
		: m_physicsActor(actor)
	{

	}

	PhysicsStaticComponent::~PhysicsStaticComponent()
	{
	}


	PhysicsDynamicComponent::~PhysicsDynamicComponent()
	{
		
	}

	PhysicsDynamicComponent::PhysicsDynamicComponent(physx::PxRigidDynamic* actor, const float3& scale)
		: m_physicsActor(actor)
	{
		m_PxTransform = actor->getGlobalPose();
		UpdatePxTransform(m_PxTransform);
		XMStoreFloat4x4(&m_Scale, XMMatrixScalingFromVector(XMVectorSet(scale.x, scale.y, scale.z, 1.0f)));
	}

	void PhysicsDynamicComponent::UpdatePxTransform(const physx::PxTransform& _pxTransform)
	{
		m_PxTransform  = _pxTransform;

		auto _translateMatrix = XMMatrixTranslation(_pxTransform.p.x, _pxTransform.p.y, _pxTransform.p.z);
		auto _rotationMatrix = XMMatrixRotationQuaternion(XMVectorSet(_pxTransform.q.x, _pxTransform.q.y, _pxTransform.q.z, _pxTransform.q.w));
		auto _scaleMatrix = XMLoadFloat4x4(&m_Scale);

		XMStoreFloat4x4(&m_Transform, _scaleMatrix * _rotationMatrix * _translateMatrix);
	}

}