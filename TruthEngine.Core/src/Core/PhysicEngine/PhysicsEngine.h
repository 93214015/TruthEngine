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

namespace TruthEngine
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
		TEPhysicsRigidBoxDesc(const float3& halfSize, const TEPhysicsRigidDesc& rigidDesc)
			: TEPhysicsRigidDesc(rigidDesc), mHalfSize(halfSize)
		{}

		float3 mHalfSize{ .0f, .0f,.0f };
	};

	struct TEPhysicsRigidSphereDesc : TEPhysicsRigidDesc
	{
		TEPhysicsRigidSphereDesc() = default;
		TEPhysicsRigidSphereDesc(float radius, const TEPhysicsRigidDesc& rigidDesc)
			: TEPhysicsRigidDesc(rigidDesc), mRadius(radius)
		{}

		float mRadius = .0f;
	};

	struct TEPhysicsRigidTriangleMeshDesc : TEPhysicsRigidDesc
	{
		TEPhysicsRigidTriangleMeshDesc() = default;
		TEPhysicsRigidTriangleMeshDesc(uint32_t vertexNum, void* vertexData, const size_t vertexStride, uint32_t triangleNum, void* indexData, const TEPhysicsRigidDesc& rigidDesc, const float3& scale = float3{1.0f, 1.0f, 1.0f})
			: TEPhysicsRigidDesc(rigidDesc), mScale(scale)
		{
			mTriangleMeshDesc.points.count = vertexNum;
			mTriangleMeshDesc.points.data = vertexData;
			mTriangleMeshDesc.points.stride = vertexStride;

			mTriangleMeshDesc.triangles.count = triangleNum;
			mTriangleMeshDesc.triangles.data = indexData;
			mTriangleMeshDesc.triangles.stride = 3 * sizeof(uint32_t);

		}

		physx::PxTriangleMeshDesc mTriangleMeshDesc;
		float3 mScale;

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
		bool Reset();

		physx::PxRigidStatic* AddRigidStaticPlane(const TEPhysicsRigidPlaneDesc& rigidPlaneDesc, Entity entity);
		physx::PxRigidStatic* AddRigidStaticBox(const TEPhysicsRigidBoxDesc& rigidBoxDesc, Entity entity);
		physx::PxRigidStatic* AddRigidStaticSphere(const TEPhysicsRigidSphereDesc& rigidSphereDesc, Entity entity);
		physx::PxRigidStatic* AddRigidStaticConvex(Entity entity);
		physx::PxRigidStatic* AddRigidStaticTriangleMesh(const TEPhysicsRigidTriangleMeshDesc& rigidTriangleMeshDesc, Entity entity);

		physx::PxRigidDynamic* AddRigidDynamicBox(const TEPhysicsRigidBoxDesc& rigidBoxDesc, Entity entity);
		physx::PxRigidDynamic* AddRigidDynamicSphere(const TEPhysicsRigidSphereDesc& rigidSphereDesc, Entity entity);
		physx::PxRigidDynamic* AddRigidDynamicConvex(Entity entity);
		physx::PxRigidDynamic* AddRigidDynamicTriangleMesh(const TEPhysicsRigidTriangleMeshDesc& rigidTriangleMeshDesc, Entity entity);

		static PhysicsEngine* GetInstance()
		{
			static PhysicsEngine s_Instance;
			return &s_Instance;
		}


	private:

		physx::PxShape* CreatePlaneShape(const TEPhysicsRigidPlaneDesc& rigidPlaneDesc);
		physx::PxShape* CreateBoxShape(const TEPhysicsRigidBoxDesc& rigidBoxDesc, const float4& scale);
		physx::PxShape* CreateSphereShape(const TEPhysicsRigidSphereDesc& rigidSphereDesc, const float4& scale);
		physx::PxShape* CreateTriangleMeshShape(const TEPhysicsRigidTriangleMeshDesc& rigidTriangleMeshDesc);

	private:

		bool m_Stopped = true;

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

		//std::vector<Entity> m_PhysicsComponentEntities;

	};
}

#define TE_INSTANCE_PHYSICSENGINE TruthEngine::PhysicsEngine::GetInstance()
