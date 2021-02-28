#pragma once

#include "PxPhysicsAPI.h"

namespace TruthEngine
{

	class PhysicsStaticComponent
	{
	public:
		PhysicsStaticComponent(physx::PxRigidStatic* actor);
		~PhysicsStaticComponent();

		physx::PxRigidStatic* GetActor()
		{
			return m_physicsActor;
		}

	private:


	private:
		physx::PxRigidStatic* m_physicsActor = nullptr;

	};


	class PhysicsDynamicComponent
	{
	public:
		PhysicsDynamicComponent(physx::PxRigidDynamic* actor, const float3& scale = float3{1.0f, 1.0f, 1.0f});
		~PhysicsDynamicComponent();

		physx::PxRigidDynamic* GetActor()
		{
			return m_physicsActor;
		}

		const float4x4& GetTranform() const
		{
			return m_Transform;
		}

		void UpdatePxTransform(const physx::PxTransform& _pxTransform);
		

	private:

	private:
		physx::PxRigidDynamic* m_physicsActor = nullptr;
		physx::PxTransform m_PxTransform;
		float4x4 m_Transform = IdentityMatrix;
		float4x4 m_Scale = IdentityMatrix;
	};

}
