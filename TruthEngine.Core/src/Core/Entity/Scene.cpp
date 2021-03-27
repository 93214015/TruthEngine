#include "pch.h"
#include "Scene.h"

#include "Core/Entity/Components.h"

#include "Core/Entity/Model/ModelManager.h" // Mesh and Material Included
#include "Core/Entity/Model/AssimpLib.h"


namespace TruthEngine
{

	Scene::Scene()
		: m_EntityTree(this)
	{}

	Entity Scene::AddEntity(const char* entityTag, Entity parent, const float4x4& tranform, const float3& _WorldCenterOffset)
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>(tranform, _WorldCenterOffset);
		entity.AddComponent<TagComponent>(entityTag);
		m_EntityTree.AddNode(entity, parent);
		return entity;
	}

	TruthEngine::Entity Scene::AddEntity(const char* entityTag, const float4x4& tranform, const float3& _WorldCenterOffset)
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>(tranform, _WorldCenterOffset);
		entity.AddComponent<TagComponent>(entityTag);
		m_EntityTree.AddNode(entity);
		return entity;
	}

	Entity Scene::AddMeshEntity(const char* _MeshName, const float4x4& _Transform, Mesh* _Mesh, Material* _Material, Entity _ModelEntity, const float3& _WorldCenterOffset)
	{
		auto entity_mesh = AddEntity(_MeshName, _ModelEntity, _Transform, _WorldCenterOffset);
		entity_mesh.AddComponent<MeshComponent>(_Mesh);
		entity_mesh.AddComponent<MaterialComponent>(_Material);
		const auto& _meshAABB = _Mesh->GetBoundingBox();
		entity_mesh.AddComponent<BoundingBoxComponent>(_meshAABB);

		GetComponent<ModelComponent>(_ModelEntity).AddMeshEntity(entity_mesh);

		BoundingBox _transformedAABB;
		_meshAABB.Transform(_transformedAABB, XMLoadFloat4x4(&_Transform));

		BoundingBox& _ModelAABB = GetComponent<BoundingBoxComponent>(_ModelEntity).GetBoundingBox();
		if (_ModelAABB.Extents == float3{ 1.0f, 1.0f, 1.0f })
		{
			_ModelAABB = _transformedAABB;
		}
		else
		{
			BoundingBox::CreateMerged(_ModelAABB, _ModelAABB, _transformedAABB);
		}

		if (m_BoundingBox.Extents == float3{ 1.0f, 1.0f, 1.0f })
		{
			m_BoundingBox = _transformedAABB;
		}
		else
		{
			BoundingBox::CreateMerged(m_BoundingBox, m_BoundingBox, _transformedAABB);
		}




		return entity_mesh;
	}

	Entity Scene::AddPrimitiveMesh(const char* _MeshName, TE_PRIMITIVE_TYPE _PrimitiveType, size_t _PrimitiveSize, Entity _ModelEntity)
	{
		Mesh* _Mesh = TE_INSTANCE_MODELMANAGER->GeneratePrimitiveMesh(_PrimitiveType, _PrimitiveSize);
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

	Entity Scene::AddModelEntity(const char* modelName, const float4x4& _Transform)
	{

		//
		///Here we are trying to place new Mesh near the selected entity
		//
		float4x4 _Trans = _Transform;
		if (m_SelectedEntity)
		{
			if (HasComponent<BoundingBoxComponent>(m_SelectedEntity))
			{
				const BoundingBox& _AABB = GetComponent<BoundingBoxComponent>(m_SelectedEntity).GetBoundingBox();
				const float4x4& _transformOffset = GetComponent<TransformComponent>(m_SelectedEntity).GetTransform();
				_Trans._41 += _transformOffset._41;
				_Trans._42 += _transformOffset._42 + (_AABB.Extents.y * 2.0);
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

		BoundingBox _AABB{};


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

	float4x4 Scene::GetTransformHierarchy(Entity entity)
	{
		if (!entity)
			return IdentityMatrix;

		const float4x4* _Transform = nullptr;

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

	float4x4 Scene::GetTransformHierarchy(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		const float4x4* _Transform = nullptr;

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

	const BoundingBox& Scene::GetBoundingBox() const noexcept
	{

		return m_BoundingBox;
	}

	void Scene::UpdateBoundingBox(const BoundingBox& _boundingBox)
	{
		if (m_BoundingBox.Contains(_boundingBox) == DirectX::CONTAINS)
			return;

		BoundingBox::CreateMerged(m_BoundingBox, m_BoundingBox, _boundingBox);
	}

	float4x4 Scene::GetParentTransforms(Entity parent)
	{
		if (!parent)
			return IdentityMatrix;

		auto& t = parent.GetComponent<TransformComponent>().GetTransform();

		return GetParentTransforms(m_EntityTree.m_Tree.find(parent)->second.mParent) * t;
	}

	float4x4 Scene::GetParentTransforms(entt::entity parentHandler)
	{
		if (parentHandler == entt::null)
			return IdentityMatrix;

		auto& t = m_Registery.get<TransformComponent>(parentHandler).GetTransform();

		return GetParentTransforms(m_EntityTree.m_Tree.find(static_cast<uint32_t>(parentHandler))->second.mParent) * t;
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

		std::string name = meshEntity.GetComponent<TagComponent>().GetTag();

		if (name.find("_Copy") != std::string::npos)
		{
			name = name + std::to_string(s_copyIndex);
		}
		else
		{
			name = name + "_Copy" + std::to_string(s_copyIndex);
		}

		const TransformComponent& _TransformComponent = meshEntity.GetComponent<TransformComponent>();
		const float4x4& transform = _TransformComponent.GetTransform();
		const float3& _worldCenterOffset = _TransformComponent.GetWorldCenterOffset();

		auto mesh = meshEntity.GetComponent<MeshComponent>().GetMesh();
		auto newMesh = TE_INSTANCE_MODELMANAGER->CopyMesh(mesh);
		auto material = meshEntity.GetComponent<MaterialComponent>().GetMaterial();
		auto newMaterial = TE_INSTANCE_MATERIALMANAGER->AddMaterial(material);

		Entity _ModelEntity = GetParent(meshEntity);

		auto _newMeshEntity = AddMeshEntity(name.c_str(), transform, mesh, material, _ModelEntity, _worldCenterOffset);

		SelectEntity(_newMeshEntity);

		s_copyIndex++;

		return _newMeshEntity;
	}

	void Scene::ImportModel(const char* filePath, std::string _ModelName)
	{
		std::vector<ImportedMeshMaterials> _ImportedData = AssimpLib::GetInstance()->ImportModel(filePath, _ModelName.c_str());

		Entity _ModelEntity = AddModelEntity(_ModelName.c_str(), IdentityMatrix);
		ModelComponent& _ModelComponent = GetComponent<ModelComponent>(_ModelEntity);
		BoundingBox& _ModelAABB = GetComponent<BoundingBoxComponent>(_ModelEntity).GetBoundingBox();

		TE_INSTANCE_MODELMANAGER->InitVertexAndIndexBuffer();

		for (ImportedMeshMaterials& _Data : _ImportedData)
		{
			const auto& _MeshAABB = _Data.mMesh->GetBoundingBox();
			const float3 _WorldCenterOffset = _MeshAABB.Center;
			Entity _MeshEntity = AddMeshEntity(_Data.mName.c_str(), IdentityMatrix, _Data.mMesh, _Data.mMaterial, _ModelEntity, _WorldCenterOffset);

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
				BoundingBox::CreateMerged(_ModelAABB, _ModelAABB, _MeshAABB);
			}

			_ModelComponent.AddMeshEntity(_MeshEntity);
		}
	}

	float3 Scene::GetPosition(Entity entity)
	{
		const auto& _transform = GetTransformHierarchy(entity);
		float3 _worldPostition = { 0.0f, 0.0f, 0.0f };
		if (HasComponent<BoundingBoxComponent>(entity))
		{
			const auto& _aabb = GetComponent<BoundingBoxComponent>(entity).GetBoundingBox();

			_worldPostition.x += _aabb.Center.x;
			_worldPostition.y += _aabb.Center.y;
			_worldPostition.z += _aabb.Center.z;
		}

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

}