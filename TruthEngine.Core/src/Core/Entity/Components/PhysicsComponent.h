#pragma once

#include "PxPhysicsAPI.h"

namespace TruthEngine::Core
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
		PhysicsDynamicComponent(physx::PxRigidDynamic* actor);
		~PhysicsDynamicComponent();
		
		physx::PxRigidDynamic* GetActor()
		{
			return m_physicsActor;
		}

	private:

	private:
		physx::PxRigidDynamic* m_physicsActor = nullptr;
	};

}
