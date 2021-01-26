#include "pch.h"
#include "PhysicsEngine.h"

#include "extensions/PxDefaultCpuDispatcher.h"

#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Components.h"
#include "../Application.h"

using namespace physx;
using namespace DirectX;


namespace TruthEngine::Core
{

	PhysicsEngine::PhysicsEngine()
	{

	}

	void PhysicsEngine::Init()
	{

		m_pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_pxDefaultAllocator, m_pxDefaultErrorCallback);

		if (!m_pxFoundation)
		{
			TE_LOG_CORE_ERROR("Create Physx::pxFoundation Failed!");
			throw;
		}

		PxTolerancesScale tol;
		tol.length = 1.0f;
		tol.speed = 10.0f;

		m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pxFoundation, tol);

		if (!m_pxPhysics)
		{
			TE_LOG_CORE_ERROR("Create Physx::pxPhysx Failed!");
			throw;
		}

		PxSceneDesc sceneDesc(m_pxPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(.0f, -9.81f, .0f);
		m_pxCpuDispatcher = PxDefaultCpuDispatcherCreate(6);
		sceneDesc.cpuDispatcher = m_pxCpuDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;

		m_pxScene = m_pxPhysics->createScene(sceneDesc);
		if (!m_pxScene)
		{
			TE_LOG_CORE_ERROR("Create Physx::pxScene Failed!");
			throw;
		}

		m_pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pxFoundation, PxCookingParams(m_pxPhysics->getTolerancesScale()));
		if (!m_pxCooking)
		{
			TE_LOG_CORE_ERROR("Creating Physx::pxCooking Failed!");
			throw;
		}

		/*if (!PxInitExtensions(*m_pxPhysics, nullptr))
		{
			TE_LOG_CORE_ERROR("pxInitExtensions Failed!");
			throw;
		}*/


		auto material = m_pxPhysics->createMaterial(.1f, .1f, 0.6f);

		/*auto modelmanager = ModelManager::GetInstance().get();
		m_planeEntity = modelmanager->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::PLANE, 20.0, IdentityMatrix);

		PxPlane pxPlane(0.0, 1.0, 0.0, 0);

		auto planeShape = m_pxPhysics->createShape(PxPlaneGeometry(), &material, true);
		pPlaneActor = m_pxPhysics->createRigidStatic(PxTransformFromPlaneEquation(pxPlane));
		pPlaneActor->attachShape(*planeShape);

		m_pxScene->addActor(*pPlaneActor);*/


		m_PhysicsComponentEntities.reserve(1000);
	}

	PhysicsEngine::~PhysicsEngine()
	{
		m_pxPhysics->release();
		m_pxFoundation->release();
	}

	bool PhysicsEngine::Play()
	{
		if (!m_Stopped)
		{
			TE_LOG_CORE_TRACE("The Physics Engine is running already!");
			return false;
		}

		m_Stopped = false;

		/*auto& transform = m_sphereEntity.GetComponent<TransformComponent>();*/

		XMVECTOR xmTranslate;
		XMVECTOR xmQuat;
		XMVECTOR xmScale;
		float4 translate; float4 quat;

		auto activeScene = TE_INSTANCE_APPLICATION->GetActiveScene();


		auto view_dynamic = activeScene->ViewEntities<PhysicsDynamicComponent>();
		for (auto entity : view_dynamic)
		{
			auto transform = activeScene->CalcTransformsToRoot(entity);

			DirectX::XMMatrixDecompose(&xmScale, &xmQuat, &xmTranslate, Math::ToXM(transform));

			DirectX::XMStoreFloat4(&translate, xmTranslate);
			DirectX::XMStoreFloat4(&quat, xmQuat);

			auto& component = activeScene->GetComponent<PhysicsDynamicComponent>(entity);

			auto dynamicActor = component.GetActor();

			if (activeScene->HasComponent<BoundingBoxComponent>(entity))
			{
				auto& aabb = activeScene->GetComponent<BoundingBoxComponent>(entity).GetBoundingBox();

				translate.x += aabb.Center.x;
				translate.y += aabb.Center.y;
				translate.z += aabb.Center.z;
			}

			PxTransform pxTrans({ translate.x, translate.y, translate.z }, { quat.x, quat.y, quat.z, quat.w });


			dynamicActor->setGlobalPose(pxTrans);
			
		}

		auto view_staic = activeScene->ViewEntities<PhysicsStaticComponent>();

		for (auto entity : view_staic)
		{
			auto transform = activeScene->CalcTransformsToRoot(entity);

			DirectX::XMMatrixDecompose(&xmScale, &xmQuat, &xmTranslate, Math::ToXM(transform));

			DirectX::XMStoreFloat4(&translate, xmTranslate);
			DirectX::XMStoreFloat4(&quat, xmQuat);

			if (activeScene->HasComponent<BoundingBoxComponent>(entity))
			{
				auto& aabb = activeScene->GetComponent<BoundingBoxComponent>(entity).GetBoundingBox();

				translate.x += aabb.Center.x;
				translate.y += aabb.Center.y;
				translate.z += aabb.Center.z;
			}

			auto staticActor = activeScene->GetComponent<PhysicsStaticComponent>(entity).GetActor();
			staticActor->setGlobalPose(PxTransform({ translate.x, translate.y, translate.z }, { quat.x, quat.y, quat.z, quat.w }));
		}

		return true;
	}

	bool PhysicsEngine::Stop()
	{
		if (m_Stopped)
		{
			TE_LOG_CORE_TRACE("The Physics Engine is stopped already!");
			return false;
		}

		m_Stopped = true;

		return true;
	}

	bool PhysicsEngine::Simulate(double dt)
	{
		if (m_Stopped)
			return false;

		static auto mAccumulator = 0.0f;
		static auto  mStepSize = 1.0f / 60.0f;

		mAccumulator += dt;
		if (mAccumulator < mStepSize)
			return false;

		mAccumulator -= mStepSize;

		m_pxScene->simulate(mStepSize);

		m_pxScene->fetchResults(true);

		uint32_t actorsNum;
		auto actors = m_pxScene->getActiveActors(actorsNum);

		for (uint32_t i = 0; i < actorsNum; ++i)
		{
			PxRigidActor* actor = (PxRigidActor*)actors[i];

			Entity* ent = (Entity*)actor->userData;

			
			auto& actorPos = actor->getGlobalPose();
			auto m = XMMatrixAffineTransformation(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(actorPos.q.x, actorPos.q.y, actorPos.q.z, actorPos.q.w), XMVectorSet(actorPos.p.x, actorPos.p.y, actorPos.p.z, 1.0f));
			auto& transform = ent->GetComponent<TransformComponent>().GetTransform();
			XMStoreFloat4x4(&transform, m);
		}

		return true;
	}


	physx::PxRigidStatic* PhysicsEngine::AddRigidStaticPlane(const TEPhysicsRigidPlaneDesc& rigidPlaneDesc, Entity entity)
	{
		PxPlane pxPlane(rigidPlaneDesc.mNormalX, rigidPlaneDesc.mNormalY, rigidPlaneDesc.mNormalZ, rigidPlaneDesc.mDistance);

		auto actorStatic = m_pxPhysics->createRigidStatic(PxTransformFromPlaneEquation(pxPlane));
		auto shapePlane = CreatePlaneShape(rigidPlaneDesc);
		actorStatic->attachShape(*shapePlane);

		actorStatic->userData = &m_PhysicsComponentEntities.emplace_back(entity);
		shapePlane->release();

		m_pxScene->addActor(*actorStatic);

		return actorStatic;
	}

	physx::PxRigidStatic* PhysicsEngine::AddRigidStaticBox(const TEPhysicsRigidBoxDesc& rigidBoxDesc, Entity entity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidBoxDesc.mTransform, _scale, _translate, _quaternion);
		auto actorStatic = m_pxPhysics->createRigidStatic(PxTransform(_translate, _quaternion));
		auto shapeBox = CreateBoxShape(rigidBoxDesc, _scale);
		actorStatic->attachShape(*shapeBox);
		actorStatic->userData = &m_PhysicsComponentEntities.emplace_back(entity);
		shapeBox->release();

		m_pxScene->addActor(*actorStatic);

		return actorStatic;
	}
	physx::PxRigidStatic* PhysicsEngine::AddRigidStaticSphere(const TEPhysicsRigidSphereDesc& rigidSphereDesc, Entity entity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidSphereDesc.mTransform, _scale, _translate, _quaternion);
		auto actorStatic = m_pxPhysics->createRigidStatic(PxTransform(_translate, _quaternion));
		auto shapeSphere = CreateSphereShape(rigidSphereDesc, _scale);
		actorStatic->attachShape(*shapeSphere);
		actorStatic->userData = &m_PhysicsComponentEntities.emplace_back(entity);
		shapeSphere->release();

		m_pxScene->addActor(*actorStatic);

		return actorStatic;
	}


	physx::PxRigidDynamic* PhysicsEngine::AddRigidDynamicBox(const TEPhysicsRigidBoxDesc& rigidBoxDesc, Entity entity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidBoxDesc.mTransform, _scale, _translate, _quaternion);
		auto actorDynamic = m_pxPhysics->createRigidDynamic(PxTransform(_translate, _quaternion));
		auto shapeBox = CreateBoxShape(rigidBoxDesc, _scale);
		actorDynamic->attachShape(*shapeBox);
		PxRigidBodyExt::updateMassAndInertia(*actorDynamic, 10.0f);
		actorDynamic->userData = &m_PhysicsComponentEntities.emplace_back(entity);
		shapeBox->release();

		m_pxScene->addActor(*actorDynamic);

		return actorDynamic;
	}
	physx::PxRigidDynamic* PhysicsEngine::AddRigidDynamicSphere(const TEPhysicsRigidSphereDesc& rigidSphereDesc, Entity entity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidSphereDesc.mTransform, _scale, _translate, _quaternion);
		auto actorDynamic = m_pxPhysics->createRigidDynamic(PxTransform(_translate, _quaternion));
		auto shapeSphere = CreateSphereShape(rigidSphereDesc, _scale);
		actorDynamic->attachShape(*shapeSphere);
		PxRigidBodyExt::updateMassAndInertia(*actorDynamic, 10.0f);
		actorDynamic->userData = &m_PhysicsComponentEntities.emplace_back(entity);
		shapeSphere->release();

		m_pxScene->addActor(*actorDynamic);

		return actorDynamic;
	}


	physx::PxShape* PhysicsEngine::CreatePlaneShape(const TEPhysicsRigidPlaneDesc& rigidPlaneDesc)
	{
		PxMaterial* material = m_pxPhysics->createMaterial(rigidPlaneDesc.mStaticFriction, rigidPlaneDesc.mDynamicFriction, rigidPlaneDesc.mRestitution);

		return m_pxPhysics->createShape(PxPlaneGeometry(), *material, true);
	}

	physx::PxShape* PhysicsEngine::CreateBoxShape(const TEPhysicsRigidBoxDesc& rigidBoxDesc, const float4& scale)
	{
		PxMaterial* material = m_pxPhysics->createMaterial(rigidBoxDesc.mStaticFriction, rigidBoxDesc.mDynamicFriction, rigidBoxDesc.mRestitution);
		
		float halfX = rigidBoxDesc.mHalfX == 0.0 ? 0.5f : rigidBoxDesc.mHalfX;
		float halfY = rigidBoxDesc.mHalfY == 0.0 ? 0.5f : rigidBoxDesc.mHalfY;
		float halfZ = rigidBoxDesc.mHalfZ == 0.0 ? 0.5f : rigidBoxDesc.mHalfZ;

		return m_pxPhysics->createShape(PxBoxGeometry(halfX * scale.x, halfY * scale.y, halfZ * scale.z), *material, true);
	}

	physx::PxShape* PhysicsEngine::CreateSphereShape(const TEPhysicsRigidSphereDesc& rigidSphereDesc, const float4& scale)
	{
		PxMaterial* material = m_pxPhysics->createMaterial(rigidSphereDesc.mStaticFriction, rigidSphereDesc.mDynamicFriction, rigidSphereDesc.mRestitution);

		float radius = rigidSphereDesc.mRadius == 0 ? 0.5f : rigidSphereDesc.mRadius;

		return m_pxPhysics->createShape(PxSphereGeometry(radius * scale.x), *material, true);
	}

}

