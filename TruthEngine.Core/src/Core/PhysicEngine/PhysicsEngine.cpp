#include "pch.h"
#include "PhysicsEngine.h"

#include "extensions/PxDefaultCpuDispatcher.h"

#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Components.h"
#include "Core/Entity/Scene.h"
#include "../Application.h"

using namespace physx;
using namespace DirectX;


namespace TruthEngine
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
		sceneDesc.gravity = PxVec3(.0f, -90.81f, .0f);
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


		PxCookingParams cookingParams(m_pxPhysics->getTolerancesScale());
		// disable mesh cleaning - perform mesh validation on development configurations
		//cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		// disable edge precompute, edges are set for each triangle, slows contact generation
		//cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		// lower hierarchy for internal mesh
		//cookingParams.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

		m_pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pxFoundation, cookingParams);
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

		/*auto modelmanager = ModelManager::GetInstance();
		m_planeEntity = modelmanager->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::PLANE, 20.0, IdentityMatrix);

		PxPlane pxPlane(0.0, 1.0, 0.0, 0);

		auto planeShape = m_pxPhysics->createShape(PxPlaneGeometry(), &material, true);
		pPlaneActor = m_pxPhysics->createRigidStatic(PxTransformFromPlaneEquation(pxPlane));
		pPlaneActor->attachShape(*planeShape);

		m_pxScene->addActor(*pPlaneActor);*/


		//m_PhysicsComponentEntities.reserve(1000);
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

		static double mAccumulator = 0.0;
		static double _TimeStep = 1.0 / 60.0;

		mAccumulator += dt;
		if (mAccumulator < _TimeStep)
			return false;

		mAccumulator = 0.0;

		m_pxScene->simulate(_TimeStep);

		m_pxScene->fetchResults(true);

		uint32_t actorsNum;
		auto actors = m_pxScene->getActiveActors(actorsNum);

		for (uint32_t i = 0; i < actorsNum; ++i)
		{
			PxRigidActor* _actor = (PxRigidActor*)actors[i];

			PhysicsDynamicComponent& _physicsComponent = GetActiveScene()->GetComponent<PhysicsDynamicComponent>((entt::entity)(reinterpret_cast<uint32_t>(_actor->userData)));

			const auto actorPos = _actor->getGlobalPose();

			_physicsComponent.UpdatePxTransform(actorPos);

			//auto& transform = ent->GetComponent<TransformComponent>().GetTransform();
			//auto _XMTranslate = XMMatrixTranslation(meshPos.x - actorPos.p.x, meshPos.y - actorPos.p.y, meshPos.z - actorPos.p.z);
			//auto _XMRotate = XMMatrixRotationQuaternion(XMVectorSet(actorPos.q.x, actorPos.q.y, actorPos.q.z, actorPos.q.w));
			//auto _XMTransform = XMLoadFloat4x4(&transform);
			////auto m = XMMatrixAffineTransformation(DirectX::XMVectorSet(transform._11, transform._22, transform._33, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(actorPos.q.x, actorPos.q.y, actorPos.q.z, actorPos.q.w), XMVectorSet(actorPos.p.x, actorPos.p.y, actorPos.p.z, 1.0f));
			//XMStoreFloat4x4(&transform, XMMatrixMultiply(_XMTransform, XMMatrixMultiply(_XMRotate, _XMTranslate)));

		}

		return true;
	}


	/*bool PhysicsEngine::Play()
	{
		if (!m_Stopped)
		{
			TE_LOG_CORE_TRACE("The Physics Engine is running already!");
			return false;
		}

		m_Stopped = false;

		/ *auto& transform = m_sphereEntity.GetComponent<TransformComponent>();* /

		XMVECTOR xmTranslate;
		XMVECTOR xmQuat;
		XMVECTOR xmScale;
		float4 translate; float4 quat;

		auto activeScene = GetActiveScene();


		auto view_dynamic = activeScene->ViewEntities<PhysicsDynamicComponent>();
		for (auto entity : view_dynamic)
		{
			auto transform = activeScene->GetTransformHierarchy(entity);

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
			auto transform = activeScene->GetTransformHierarchy(entity);

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
	}*/




	physx::PxRigidStatic* PhysicsEngine::AddRigidStaticPlane(const TEPhysicsRigidPlaneDesc& rigidPlaneDesc, Entity entity)
	{
		PxPlane pxPlane(rigidPlaneDesc.mNormalX, rigidPlaneDesc.mNormalY, rigidPlaneDesc.mNormalZ, rigidPlaneDesc.mDistance);

		auto actorStatic = m_pxPhysics->createRigidStatic(PxTransformFromPlaneEquation(pxPlane));
		auto shapePlane = CreatePlaneShape(rigidPlaneDesc);
		actorStatic->attachShape(*shapePlane);
		shapePlane->release();

		auto physicsComponent = &entity.AddComponent<PhysicsStaticComponent>(actorStatic);
		actorStatic->userData = physicsComponent;

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
		shapeBox->release();

		auto physicsComponent = &entity.AddComponent<PhysicsStaticComponent>(actorStatic);
		actorStatic->userData = physicsComponent;

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
		shapeSphere->release();

		auto physicsComponent = &entity.AddComponent<PhysicsStaticComponent>(actorStatic);
		actorStatic->userData = physicsComponent;


		m_pxScene->addActor(*actorStatic);

		return actorStatic;
	}



	physx::PxRigidDynamic* PhysicsEngine::AddRigidDynamicBox(const TEPhysicsRigidBoxDesc& rigidBoxDesc, Entity entity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidBoxDesc.mTransform, _scale, _translate, _quaternion);
		PxRigidDynamic* actorDynamic = m_pxPhysics->createRigidDynamic(PxTransform(_translate, _quaternion));
		PxShape* shapeBox = CreateBoxShape(rigidBoxDesc, _scale);
		actorDynamic->attachShape(*shapeBox);

		//PxRigidBodyExt::updateMassAndInertia(*actorDynamic, 1000.0f);		

		auto _MassProperty = PxRigidBodyExt::computeMassPropertiesFromShapes(&shapeBox, 1);

		shapeBox->release();

		auto physicsComponent = &entity.AddComponent<PhysicsDynamicComponent>(actorDynamic, reinterpret_cast<const float3&>(_scale));
		actorDynamic->userData = (void*)static_cast<uint32_t>(entity);

		m_pxScene->addActor(*actorDynamic);

		return actorDynamic;
	}
	physx::PxRigidDynamic* PhysicsEngine::AddRigidDynamicSphere(const TEPhysicsRigidSphereDesc& rigidSphereDesc, Entity entity, std::optional<float3> _LinearVelocity, std::optional<float3> _AngularVelocity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidSphereDesc.mTransform, _scale, _translate, _quaternion);
		PxRigidDynamic* actorDynamic = m_pxPhysics->createRigidDynamic(PxTransform(_translate, _quaternion));
		PxShape* shapeSphere = CreateSphereShape(rigidSphereDesc, _scale);
		actorDynamic->attachShape(*shapeSphere);
		shapeSphere->release();

		if (_LinearVelocity.has_value())
		{
			actorDynamic->setLinearVelocity(*_LinearVelocity);
		}
		if (_AngularVelocity.has_value())
		{
			actorDynamic->setAngularVelocity(*_AngularVelocity);
		}
		//PxRigidBodyExt::updateMassAndInertia(*actorDynamic, 1000.0f);


		auto physicsComponent = &entity.AddComponent<PhysicsDynamicComponent>(actorDynamic, reinterpret_cast<const float3&>(_scale));
		actorDynamic->userData = (void*)static_cast<uint32_t>(entity);

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

		float halfX = rigidBoxDesc.mHalfSize.x == 0.0 ? 0.5f : rigidBoxDesc.mHalfSize.x;
		float halfY = rigidBoxDesc.mHalfSize.y == 0.0 ? 0.5f : rigidBoxDesc.mHalfSize.y;
		float halfZ = rigidBoxDesc.mHalfSize.z == 0.0 ? 0.5f : rigidBoxDesc.mHalfSize.z;

		return m_pxPhysics->createShape(PxBoxGeometry(halfX * scale.x, halfY * scale.y, halfZ * scale.z), *material, true);
	}
	physx::PxShape* PhysicsEngine::CreateSphereShape(const TEPhysicsRigidSphereDesc& rigidSphereDesc, const float4& scale)
	{
		PxMaterial* material = m_pxPhysics->createMaterial(rigidSphereDesc.mStaticFriction, rigidSphereDesc.mDynamicFriction, rigidSphereDesc.mRestitution);

		float radius = rigidSphereDesc.mRadius == 0 ? 0.5f : rigidSphereDesc.mRadius;

		return m_pxPhysics->createShape(PxSphereGeometry(radius * scale.x), *material, true);
	}

	physx::PxShape* PhysicsEngine::CreateTriangleMeshShape(const TEPhysicsRigidTriangleMeshDesc& rigidTriangleMeshDesc)
	{

#ifdef TE_DEBUG
		auto validated = m_pxCooking->validateTriangleMesh(rigidTriangleMeshDesc.mTriangleMeshDesc);
		if (!validated)
			TE_LOG_CORE_WARN("PhysicsEngine: RigidBodyMeshTriangle Validation failed!");
		//TE_ASSERT_CORE(validated, "Creation of Physics Traingle Mesh is failed!");
#endif

		PxTriangleMesh* _pxTriangleMesh = m_pxCooking->createTriangleMesh(rigidTriangleMeshDesc.mTriangleMeshDesc, m_pxPhysics->getPhysicsInsertionCallback());
		
		PxMeshScale _pxMeshScale( rigidTriangleMeshDesc.mScale);
		PxTriangleMeshGeometry _pxTriangleMeshGeo(_pxTriangleMesh, _pxMeshScale);

		PxMaterial* material = m_pxPhysics->createMaterial(rigidTriangleMeshDesc.mStaticFriction, rigidTriangleMeshDesc.mDynamicFriction, rigidTriangleMeshDesc.mRestitution);

		return m_pxPhysics->createShape(_pxTriangleMeshGeo, *material);
	}

	bool PhysicsEngine::Reset()
	{
		auto scene = GetActiveScene();

		auto& groups = scene->ViewEntities<PhysicsDynamicComponent>();

		for (auto& entity : groups)
		{
			auto& physicsComponent = scene->GetComponent<PhysicsDynamicComponent>(entity);
			//const float3& _WorldOffset = scene->GetComponent<TransformComponent>(entity).GetWorldCenterOffset();
			float4x4A staticTransform = scene->GetStaticTransformHierarchy(entity);
			/*staticTransform._41 += _WorldOffset.x;
			staticTransform._42 += _WorldOffset.y;
			staticTransform._43 += _WorldOffset.z;*/

			auto actor = physicsComponent.GetActor();

			float4 _scale, _translate, _quaternion;
			Math::DecomposeMatrix(staticTransform, _scale, _translate, _quaternion);

			const PxTransform pxTrans = PxTransform(_translate, _quaternion);

			actor->setGlobalPose(pxTrans);
			actor->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f), false);
			actor->setAngularVelocity(PxVec3(0.0f, 0.0f, 0.0f), false);

			physicsComponent.UpdatePxTransform(pxTrans);
		}

		return true;
	}

	physx::PxRigidStatic* PhysicsEngine::AddRigidStaticTriangleMesh(const TEPhysicsRigidTriangleMeshDesc& rigidTriangleMeshDesc, Entity entity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidTriangleMeshDesc.mTransform, _scale, _translate, _quaternion);
		auto staticActor = m_pxPhysics->createRigidStatic(PxTransform(_translate, _quaternion));
		auto shape = CreateTriangleMeshShape(rigidTriangleMeshDesc);
		staticActor->attachShape(*shape);
		shape->release();

		m_pxScene->addActor(*staticActor);

		return staticActor;
	}

	physx::PxRigidDynamic* PhysicsEngine::AddRigidDynamicTriangleMesh(const TEPhysicsRigidTriangleMeshDesc& rigidTriangleMeshDesc, Entity entity)
	{
		float4 _translate, _quaternion, _scale;
		Math::DecomposeMatrix(rigidTriangleMeshDesc.mTransform, _scale, _translate, _quaternion);
		auto dynamicActor = m_pxPhysics->createRigidDynamic(PxTransform(_translate, _quaternion));
		dynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		auto shape = CreateTriangleMeshShape(rigidTriangleMeshDesc);
		dynamicActor->attachShape(*shape);
		shape->release();

		m_pxScene->addActor(*dynamicActor);

		auto physicsComponent = entity.AddComponent<PhysicsDynamicComponent>(dynamicActor, reinterpret_cast<const float3&>(_scale));
		dynamicActor->userData = (void*)static_cast<uint32_t>(entity);

		return dynamicActor;
	}

}

