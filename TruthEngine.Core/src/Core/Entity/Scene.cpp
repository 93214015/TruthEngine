#include "pch.h"
#include "Scene.h"

#include "Core/Entity/Components.h"

#include "Core/Entity/Model/ModelManager.h" // Mesh and Material Included


namespace TruthEngine
{

	Scene::Scene()
		: m_EntityTree(this)
	{}

	Entity Scene::AddEntity(const char* entityTag, Entity parent, const float4x4& tranform)
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>(tranform);
		entity.AddComponent<TagComponent>(entityTag);
		m_EntityTree.AddNode(entity, parent);
		return entity;
	}

	TruthEngine::Entity Scene::AddEntity(const char* entityTag, const float4x4& tranform)
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>(tranform);
		entity.AddComponent<TagComponent>(entityTag);
		m_EntityTree.AddNode(entity);
		return entity;
	}

	Entity Scene::AddMeshEntity(const char* meshName, const float4x4& transform, Mesh* mesh, Material* material, Entity parentEntity)
	{
		auto entity_mesh = AddEntity(meshName, parentEntity, transform);
		entity_mesh.AddComponent<MeshComponent>(mesh);
		entity_mesh.AddComponent<MaterialComponent>(material);
		const auto& _meshAABB = mesh->GetBoundingBox();
		entity_mesh.AddComponent<BoundingBoxComponent>(_meshAABB);

		BoundingBox _transformedAABB;
		_meshAABB.Transform(_transformedAABB, XMLoadFloat4x4(&transform));

		BoundingBox::CreateMerged(m_BoundingBox, m_BoundingBox, _transformedAABB);

		return entity_mesh;
	}

	TruthEngine::Entity Scene::AddEnvironmentEntity()
	{
		auto entity_environment = AddEntity("EnvironmentSphere", Entity());

		Mesh* _Mesh = nullptr;

		TE_INSTANCE_MODELMANAGER->GenerateEnvironmentMesh(&_Mesh);
		entity_environment.AddComponent<EnvironmentComponent>(_Mesh);

		return entity_environment;
	}

	std::vector<TruthEngine::Entity> Scene::GetAncestor(const Entity& entity)
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

	float4x4 Scene::CalcTransformsToRoot(Entity& entity)
	{
		if (!entity)
			return IdentityMatrix;

		auto& t = entity.GetComponent<TransformComponent>().GetTransform();
		auto itr = m_EntityTree.m_Tree.find(entity);
		return CalcTransformsToRoot(itr->second.mParent) * t;
	}

	float4x4 Scene::CalcTransformsToRoot(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		auto& t = m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return CalcTransformsToRoot(itr->second.mParent) * t;
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


		auto& transform = meshEntity.GetComponent<TransformComponent>().GetTransform();

		auto _newMeshEntity = AddEntity(name.c_str(), Entity(), transform);

		auto mesh = meshEntity.GetComponent<MeshComponent>().GetMesh();
		auto newMesh = TE_INSTANCE_MODELMANAGER->CopyMesh(mesh);

		auto material = meshEntity.GetComponent<MaterialComponent>().GetMaterial();

		auto newMaterial = TE_INSTANCE_MODELMANAGER->GetMaterialManager()->AddMaterial(material);

		_newMeshEntity.AddComponent<MeshComponent>(newMesh);
		_newMeshEntity.AddComponent<MaterialComponent>(newMaterial);
		_newMeshEntity.AddComponent<BoundingBoxComponent>(newMesh->GetBoundingBox());

		SelectEntity(_newMeshEntity);

		s_copyIndex++;

		return _newMeshEntity;
	}

	float3 Scene::GetPosition(Entity entity)
	{
		const auto& _transform = CalcTransformsToRoot(entity);
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
		const auto& _transform = CalcTransformsToRoot(entityHandle);
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