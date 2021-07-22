#include "pch.h"
#include "Scene.h"

#include "Core/Application.h"

#include "Core/Event/EventKey.h"

#include "Core/Entity/Components.h"
#include "Core/Entity/Model/ModelManager.h" // Mesh and Material Included
#include "Core/Entity/Model/AssimpLib.h"
#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Camera/CameraManager.h"

#include "Core/PhysicEngine/PhysicsEngine.h"

namespace TruthEngine
{

	Scene::Scene()
		: m_EntityTree(this), m_LightManager(LightManager::GetInstace())
	{
	}

	void Scene::Init()
	{
		m_ModelShootedBall = AddModelEntity("ShootedBallModel", IdentityMatrix);
		RegisterEventListener();
	}

	Entity Scene::AddEntity(const char* entityTag, Entity parent, const float4x4A& tranform /*, const float3& _WorldCenterOffset*/)
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>(tranform /*, _WorldCenterOffset*/);
		entity.AddComponent<TagComponent>(entityTag);
		m_EntityTree.AddNode(entity, parent);
		return entity;
	}

	TruthEngine::Entity Scene::AddEntity(const char* entityTag, const float4x4A& tranform /*, const float3& _WorldCenterOffset*/)
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>(tranform);
		entity.AddComponent<TagComponent>(entityTag);
		m_EntityTree.AddNode(entity);
		return entity;
	}

	Entity Scene::AddMeshEntity(const char* _MeshName, const float4x4A& _Transform, const Mesh& _Mesh, Material* _Material, Entity _ModelEntity)
	{
		auto entity_mesh = AddEntity(_MeshName, _ModelEntity, _Transform);
		entity_mesh.AddComponent<MeshComponent>(_Mesh);
		entity_mesh.AddComponent<MaterialComponent>(_Material);

		//Add BoundignAABoxComponent and get the bounding box
		const BoundingAABox& _meshAABB = entity_mesh.AddComponent<BoundingBoxComponent>(_Mesh).GetBoundingBox();

		GetComponent<ModelComponent>(_ModelEntity).AddMeshEntity(entity_mesh);

		BoundingAABox _transformedAABB;
		_meshAABB.Transform(_transformedAABB, XMLoadFloat4x4A(&_Transform));

		BoundingAABox& _ModelAABB = GetComponent<BoundingBoxComponent>(_ModelEntity).GetBoundingBox();
		if (_ModelAABB.Extents == float3{ 1.0f, 1.0f, 1.0f })
		{
			_ModelAABB = _transformedAABB;
		}
		else
		{
			BoundingAABox::CreateMerged(_ModelAABB, _ModelAABB, _transformedAABB);
		}

		if (m_BoundingBox.Extents == float3{ 1.0f, 1.0f, 1.0f })
		{
			m_BoundingBox = _transformedAABB;
		}
		else
		{
			BoundingAABox::CreateMerged(m_BoundingBox, m_BoundingBox, _transformedAABB);
		}

		return entity_mesh;
	}

	Entity Scene::AddPrimitiveMesh(const char* _MeshName, TE_PRIMITIVE_TYPE _PrimitiveType, const float3& _PrimitiveSize, Entity _ModelEntity)
	{
		const Mesh& _Mesh = TE_INSTANCE_MODELMANAGER->GeneratePrimitiveMesh(_PrimitiveType, _PrimitiveSize.x, _PrimitiveSize.y, _PrimitiveSize.z);
		Material* _Material = TE_INSTANCE_MATERIALMANAGER->AddDefaultMaterial(TE_IDX_MESH_TYPE::MESH_NTT);
		return AddMeshEntity(_MeshName, IdentityMatrix, _Mesh, _Material, _ModelEntity);
	}


	TruthEngine::Entity Scene::AddEnvironmentEntity()
	{
		auto entity_environment = AddEntity("EnvironmentSphere");

		Mesh* _Mesh = nullptr;

		TE_INSTANCE_MODELMANAGER->GenerateEnvironmentMesh(&_Mesh);

		entity_environment.AddComponent<EnvironmentComponent>(_Mesh);

		return entity_environment;
	}

	Entity Scene::AddLightEntity_Directional(const std::string_view _Name, const float3& _Strength, const float3& _Direction, const float3& _Position, const float _LightSize, const uint32_t _CastShadow, const float4& _CascadesCoveringDepth)
	{
		LightDirectional* _Light = m_LightManager->AddLightDirectional(
			_Name, _Strength, _Direction, _Position, _LightSize, _CastShadow, _CascadesCoveringDepth
		);

		auto entityLight = AddEntity(_Name.data());
		entityLight.AddComponent<LightComponent>(_Light);

		return entityLight;
	}

	Entity Scene::AddLightEntity_Spot(const std::string_view _Name, const float3& _Strength, const float3& _Direction, const float3& _Position, const float _LightSize, const bool _IsCastShadow, const float _FalloffStart, const float _FalloffEnd, const float _InnerConeAngle, const float _OuterConeAngle)
	{
		LightSpot* _Light = m_LightManager->AddLightSpot(
			_Name, _Strength, _Direction, _Position, _LightSize, _IsCastShadow, _FalloffStart, _FalloffEnd, _InnerConeAngle, _OuterConeAngle
		);

		auto entityLight = AddEntity(_Name.data());
		entityLight.AddComponent<LightComponent>(_Light);

		return entityLight;
	}

	Entity Scene::AddModelEntity(const char* modelName, const float4x4A& _Transform)
	{
		//
		///Here we are trying to place new Mesh near the selected entity
		//
		float4x4A _Trans = _Transform;
		if (m_SelectedEntity)
		{
			if (HasComponent<BoundingBoxComponent>(m_SelectedEntity))
			{
				const BoundingAABox& _AABB = GetComponent<BoundingBoxComponent>(m_SelectedEntity).GetBoundingBox();
				const float4x4A& _transformOffset = GetComponent<TransformComponent>(m_SelectedEntity).GetTransform();
				_Trans._41 += _transformOffset._41;
				_Trans._42 += _transformOffset._42 + (_AABB.Extents.y + 20.0f);
				_Trans._43 += _transformOffset._43;

				float3 _v{ 0.0f, 1.0f, 0.0f };

				_v = Math::TransformVector(_v, _transformOffset);

				_Trans._41 += _v.x;
				_Trans._42 += _v.y;
				_Trans._43 += _v.z;
			}
		}


		Entity _ModelEntity = AddEntity(modelName, _Trans);
		_ModelEntity.AddComponent<ModelComponent>();


		BoundingAABox _AABB{};
		_ModelEntity.AddComponent<BoundingBoxComponent>(_AABB);


		return _ModelEntity;

	}

	std::vector<TruthEngine::Entity> Scene::GetAncestor(const Entity entity)
	{
		std::vector<Entity> ancestors;

		auto itr = m_EntityTree.m_Tree.find(entity);
		auto parentEntity = itr->second.mParent;

		while (parentEntity)
		{

			ancestors.emplace_back(parentEntity);
			itr = m_EntityTree.m_Tree.find(parentEntity);
			parentEntity = itr->second.mParent;
		}

		return ancestors;
	}

	std::vector<TruthEngine::Entity> Scene::GetAncestor(entt::entity entityHandler)
	{
		std::vector<Entity> ancestors;

		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		auto parentEntity = itr->second.mParent;

		while (parentEntity)
		{

			ancestors.emplace_back(parentEntity);
			itr = m_EntityTree.m_Tree.find(parentEntity);
			parentEntity = itr->second.mParent;
		}

		return ancestors;
	}

	float4x4A Scene::GetTransformHierarchy(Entity entity)
	{
		if (!entity)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		if (HasComponent<PhysicsDynamicComponent>(entity))
		{
			_Transform = &entity.GetComponent<PhysicsDynamicComponent>().GetTranform();
		}
		else
		{
			_Transform = &entity.GetComponent<TransformComponent>().GetTransform();
		}

		const auto itr = m_EntityTree.m_Tree.find(entity);
		return (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}

	float4x4A Scene::GetTransformHierarchy(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		if (m_Registery.has<PhysicsDynamicComponent>(entityHandler))
		{
			_Transform = &m_Registery.get<PhysicsDynamicComponent>(entityHandler).GetTranform();
		}
		else
		{
			_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		}


		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return  (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}

	float3 Scene::GetTranslateHierarchy(Entity entity)
	{
		float3 _Result = { .0f, .0f, .0f };

		if (!entity)
			return _Result;

		const float4x4A* _Transform = nullptr;

		if (HasComponent<PhysicsDynamicComponent>(entity))
		{
			_Transform = &GetComponent<PhysicsDynamicComponent>(entity).GetTranform();
		}
		else
		{
			_Transform = &GetComponent<TransformComponent>(entity).GetTransform();
		}

		_Result = _Result + float3{ _Transform->_41, _Transform->_42, _Transform->_43 };

		auto itr = m_EntityTree.m_Tree.find(entity);
		return  _Result + GetTranslateHierarchy(itr->second.mParent);
	}

	float3 Scene::GetTranslateHierarchy(entt::entity entityHandler)
	{
		float3 _Result = { .0f, .0f, .0f };

		if (entityHandler == entt::null)
			return _Result;

		const float4x4A* _Transform = nullptr;

		if (m_Registery.has<PhysicsDynamicComponent>(entityHandler))
		{
			_Transform = &m_Registery.get<PhysicsDynamicComponent>(entityHandler).GetTranform();
		}
		else
		{
			_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		}

		_Result = _Result + float3{ _Transform->_41, _Transform->_42, _Transform->_43 };

		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return  _Result + GetTranslateHierarchy(itr->second.mParent);
	}

	float4x4A Scene::GetStaticTransformHierarchy(Entity entity)
	{
		if (!entity)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		_Transform = &entity.GetComponent<TransformComponent>().GetTransform();

		const auto itr = m_EntityTree.m_Tree.find(entity);
		return (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}

	float4x4A Scene::GetStaticTransformHierarchy(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();


		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return  (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}

	const BoundingAABox& Scene::GetBoundingBox() const noexcept
	{

		return m_BoundingBox;
	}

	void Scene::UpdateBoundingBox(const BoundingAABox& _boundingBox)
	{
		/*if (m_BoundingBox.Contains(_boundingBox) == DirectX::CONTAINS)
			return;*/
		BoundingAABox::CreateMerged(m_BoundingBox, m_BoundingBox, _boundingBox);
	}

	float4x4A Scene::GetParentTransforms(Entity parent)
	{
		if (!parent)
			return IdentityMatrix;

		auto& t = parent.GetComponent<TransformComponent>().GetTransform();

		return GetParentTransforms(m_EntityTree.m_Tree.find(parent)->second.mParent) * t;
	}

	float4x4A Scene::GetParentTransforms(entt::entity parentHandler)
	{
		if (parentHandler == entt::null)
			return IdentityMatrix;

		auto& t = m_Registery.get<TransformComponent>(parentHandler).GetTransform();

		return GetParentTransforms(m_EntityTree.m_Tree.find(static_cast<uint32_t>(parentHandler))->second.mParent) * t;
	}

	Entity Scene::GetModelEntity(Entity _Entity)
	{
		if (HasComponent<ModelComponent>(_Entity))
		{
			return _Entity;
		}
		else
		{
			return GetParent(_Entity);
		}
	}

	Entity Scene::GetParent(const Entity _Entity) const
	{

		const auto _Itr = m_EntityTree.m_Tree.find(_Entity);
		if (_Itr != m_EntityTree.m_Tree.cend())
		{
			return _Itr->second.mParent;
		}

		return Entity();
	}

	std::vector<Entity> Scene::GetChildrenEntity(Entity entity)
	{
		std::vector<Entity> r;

		for (auto& node : m_EntityTree.m_Tree.find(entity)->second.mChildren)
		{
			r.emplace_back(node.mEntity);
		}

		return r;
	}

	std::vector<Entity> Scene::GetChildrenEntity(entt::entity entityHandler)
	{
		std::vector<Entity> r;

		for (auto& node : m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler))->second.mChildren)
		{
			r.emplace_back(node.mEntity);
		}

		return r;
	}

	TruthEngine::Entity Scene::CopyMeshEntity(Entity meshEntity)
	{
		static uint32_t s_copyIndex = 0;

		std::string name(meshEntity.GetComponent<TagComponent>().GetTag());

		if (name.find("_Copy") != std::string::npos)
		{
			name = name + std::to_string(s_copyIndex);
		}
		else
		{
			name = name + "_Copy" + std::to_string(s_copyIndex);
		}

		const TransformComponent& _TransformComponent = meshEntity.GetComponent<TransformComponent>();
		const float4x4A& transform = _TransformComponent.GetTransform();
		//const float3& _worldCenterOffset = _TransformComponent.GetWorldCenterOffset();

		const Mesh& mesh = meshEntity.GetComponent<MeshComponent>().GetMesh();
		auto newMesh = TE_INSTANCE_MODELMANAGER->CopyMesh(mesh);
		auto material = meshEntity.GetComponent<MaterialComponent>().GetMaterial();
		auto newMaterial = TE_INSTANCE_MATERIALMANAGER->AddMaterial(material);

		Entity _ModelEntity = GetParent(meshEntity);

		auto _newMeshEntity = AddMeshEntity(name.c_str(), transform, mesh, newMaterial, _ModelEntity);

		SelectEntity(_newMeshEntity);

		s_copyIndex++;

		return _newMeshEntity;
	}

	void Scene::ImportModel(const char* filePath, std::string _ModelName)
	{
		std::vector<ImportedMeshMaterials> _ImportedData = AssimpLib::GetInstance()->ImportModel(filePath, _ModelName.c_str());

		Entity _ModelEntity = AddModelEntity(_ModelName.c_str(), IdentityMatrix);
		ModelComponent& _ModelComponent = GetComponent<ModelComponent>(_ModelEntity);
		BoundingAABox& _ModelAABB = GetComponent<BoundingBoxComponent>(_ModelEntity).GetBoundingBox();

		TE_INSTANCE_MODELMANAGER->InitVertexAndIndexBuffer();

		for (ImportedMeshMaterials& _Data : _ImportedData)
		{

			//BoundingAABox _MeshAABB = 
			const float3 _WorldCenterOffset = _MeshAABB.Center;
			Entity _MeshEntity = AddMeshEntity(_Data.mName.c_str(), IdentityMatrix, _Data.mMesh, _Data.mMaterial, _ModelEntity);

			if (_Data.mAnimation)
			{
				_MeshEntity.AddComponent<SkinnedAnimationComponent>(_Data.mAnimation);
			}

			if (_ModelAABB.Extents == float3{ 1.0f, 1.0f, 1.0f })
			{
				_ModelAABB = _MeshAABB;
			}
			else
			{
				BoundingAABox::CreateMerged(_ModelAABB, _ModelAABB, _MeshAABB);
			}

		}
	}

	float3 Scene::GetPosition(Entity entity)
	{
		float3 _worldPostition = { 0.0f, 0.0f, 0.0f };
		if (HasComponent<BoundingBoxComponent>(entity))
		{
			const auto& _aabb = GetComponent<BoundingBoxComponent>(entity).GetBoundingBox();

			_worldPostition.x += _aabb.Center.x;
			_worldPostition.y += _aabb.Center.y;
			_worldPostition.z += _aabb.Center.z;
		}

		const auto& _transform = GetTransformHierarchy(entity);
		auto xmPos = XMVector3TransformCoord(XMLoadFloat3(&_worldPostition), XMLoadFloat4x4(&_transform));
		XMStoreFloat3(&_worldPostition, xmPos);

		return  _worldPostition;
	}

	float3 Scene::GetPosition(entt::entity entityHandle)
	{
		const auto& _transform = GetTransformHierarchy(entityHandle);
		float3 _worldPostition = { 0.0f, 0.0f, 0.0f };
		if (HasComponent<BoundingBoxComponent>(entityHandle))
		{
			const auto& _aabb = GetComponent<BoundingBoxComponent>(entityHandle).GetBoundingBox();

			_worldPostition.x += _aabb.Center.x;
			_worldPostition.y += _aabb.Center.y;
			_worldPostition.z += _aabb.Center.z;
		}

		auto xmPos = XMVector3TransformCoord(XMLoadFloat3(&_worldPostition), XMLoadFloat4x4(&_transform));
		XMStoreFloat3(&_worldPostition, xmPos);

		return  _worldPostition;
	}

	Camera* Scene::GetActiveCamera() const
	{
		return CameraManager::GetInstance()->GetActiveCamera();
	}

	void Scene::ShootTheBall()
	{
		static uint32_t _BallIndex = 0;
		std::string _BallName = "Ball" + std::to_string(_BallIndex);
		Entity _BallEntity = AddPrimitiveMesh(_BallName.c_str(), TE_PRIMITIVE_TYPE::SPHERE, float3{1.0f, 0.0f, 0.0f}, m_ModelShootedBall);

		Camera* _ActiveCamera = GetActiveCamera();
		float3 _LinearVelocity = _ActiveCamera->GetLook() * float3{ 120.0f, 120.0f, 120.0f };
		
		float4x4A _Transform = Math::TransformMatrixTranslate(_ActiveCamera->GetPosition());

		TEPhysicsRigidDesc _RigidDesc{0.5f, 0.5f, 0.5f, _Transform};
		TEPhysicsRigidSphereDesc _SphereRigidDesc{1.0f, _RigidDesc};

		TE_INSTANCE_PHYSICSENGINE->AddRigidDynamicSphere(_SphereRigidDesc, _BallEntity, _LinearVelocity);
		TE_INSTANCE_PHYSICSENGINE->Play();
	}

	void Scene::RegisterEventListener()
	{
		auto _LambdaOnKeyReleased = [this](Event& _Event) 
		{  
			OnEventKeyPressed(static_cast<EventKeyReleased&>(_Event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::KeyReleased, _LambdaOnKeyReleased);
	}

	void Scene::OnEventKeyPressed(EventKeyReleased& _event)
	{
		if (_event.GetKeyCode() == Key::K)
		{
			ShootTheBall();
		}
	}

}