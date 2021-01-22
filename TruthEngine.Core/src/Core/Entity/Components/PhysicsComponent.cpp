#include "pch.h"
#include "PhysicsComponent.h"

namespace TruthEngine::Core
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

	PhysicsDynamicComponent::PhysicsDynamicComponent(physx::PxRigidDynamic* actor)
		: m_physicsActor(actor)
	{

	}

}