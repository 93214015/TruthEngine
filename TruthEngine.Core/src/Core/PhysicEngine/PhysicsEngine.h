#pragma once


#include "Core/Entity/Entity.h"


class UserErrorCallback : public physx::PxErrorCallback
{

public:
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		TE_LOG_CORE_ERROR(message);
		throw;
	}

};

namespace TruthEngine::Core
{



	enum class TE_PHYSICS_RIGID_TYPE
	{
		STATIC,
		DYNAMIC
	};

	enum class TE_PHYSICS_RIGID_SHAPE
	{
		PLANE,
		BOX,
		SPHERE,
		CONVEX,
		TRIANGLED,
	};

	struct TEPhysicsRigidDesc
	{
		TEPhysicsRigidDesc() = default;
		TEPhysicsRigidDesc(float staticFriction, float dynamicFriction, float restitution, const float4x4& transform)
			: mStaticFriction(staticFriction), mDynamicFriction(dynamicFriction), mRestitution(restitution), mTransform(transform)
		{}

		float mStaticFriction = .0f;
		float mDynamicFriction = .0f;
		float mRestitution = .0f;

		float4x4 mTransform = IdentityMatrix;
	};

	struct TEPhysicsRigidPlaneDesc : TEPhysicsRigidDesc
	{
		TEPhysicsRigidPlaneDesc() = default;
		TEPhysicsRigidPlaneDesc(float normalX, float normalY, float normalZ, float distance, const TEPhysicsRigidDesc& rigidDesc)
			: TEPhysicsRigidDesc(rigidDesc), mNormalX(normalX), mNormalY(normalY), mNormalZ(normalZ), mDistance(distance)
		{}

		float mNormalX = .0f;
		float mNormalY = .0f;
		float mNormalZ = .0f;

		float mDistance = .0f;
	};

	struct TEPhysicsRigidBoxDesc : TEPhysicsRigidDesc
	{
		TEPhysicsRigidBoxDesc() = default;
		TEPhysicsRigidBoxDesc(float halfX, float halfY, float halfZ, const TEPhysicsRigidDesc& rigidDesc)
			: TEPhysicsRigidDesc(rigidDesc), mHalfX(halfX), mHalfY(halfY), mHalfZ(halfZ)
		{}

		float mHalfX = .0f, mHalfY = .0f, mHalfZ = .0f;
	};

	struct TEPhysicsRigidSphereDesc : TEPhysicsRigidDesc
	{
		TEPhysicsRigidSphereDesc() = default;
		TEPhysicsRigidSphereDesc(float radius, const TEPhysicsRigidDesc& rigidDesc)
			: TEPhysicsRigidDesc(rigidDesc), mRadius(radius)
		{}

		float mRadius = .0f;
	};



	class PhysicsEngine
	{
	public:
		PhysicsEngine();
		~PhysicsEngine();

		void Init();
		bool Play();
		bool Simulate(double dt);
		bool Stop();

		physx::PxRigidStatic* AddRigidStaticPlane(const TEPhysicsRigidPlaneDesc& rigidPlaneDesc, Entity entity);
		physx::PxRigidStatic* AddRigidStaticBox(const TEPhysicsRigidBoxDesc& rigidBoxDesc, Entity entity);
		physx::PxRigidStatic* AddRigidStaticSphere(const TEPhysicsRigidSphereDesc& rigidSphereDesc, Entity entity);
		physx::PxRigidStatic* AddRigidStaticConvex(Entity entity);
		physx::PxRigidStatic* AddRigidStaticTriangled(Entity entity);

		physx::PxRigidDynamic* AddRigidDynamicBox(const TEPhysicsRigidBoxDesc& rigidBoxDesc, Entity entity);
		physx::PxRigidDynamic* AddRigidDynamicSphere(const TEPhysicsRigidSphereDesc& rigidSphereDesc, Entity entity);
		physx::PxRigidDynamic* AddRigidDynamicConvex(Entity entity);
		physx::PxRigidDynamic* AddRigidDynamicTriangled(Entity entity);

		static PhysicsEngine* GetInstance()
		{
			static PhysicsEngine s_Instance;
			return &s_Instance;
		}


	private:

		physx::PxShape* CreatePlaneShape(const TEPhysicsRigidPlaneDesc& rigidPlaneDesc);
		physx::PxShape* CreateBoxShape(const TEPhysicsRigidBoxDesc& rigidBoxDesc, const float4& scale);
		physx::PxShape* CreateSphereShape(const TEPhysicsRigidSphereDesc& rigidSphereDesc, const float4& scale);

	private:

		bool m_Stopped = true;

		Entity m_planeEntity;
		Entity m_sphereEntity;

		physx::PxFoundation* m_pxFoundation;
		physx::PxPhysics* m_pxPhysics;
		physx::PxScene* m_pxScene;
		physx::PxDefaultCpuDispatcher* m_pxCpuDispatcher;
		physx::PxCooking* m_pxCooking;


		physx::PxDefaultAllocator m_pxDefaultAllocator;
		UserErrorCallback m_pxDefaultErrorCallback;


		physx::PxRigidStatic* pPlaneActor;
		physx::PxRigidStatic* pBoxActor;
		physx::PxRigidDynamic* pSphereActor;

		std::vector<Entity> m_PhysicsComponentEntities;

	};
}

#define TE_INSTANCE_PHYSICSENGINE TruthEngine::Core::PhysicsEngine::GetInstance()