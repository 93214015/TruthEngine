#include "pch.h"
#include "Scene.h"

#include "Components.h"

namespace TruthEngine::Core
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

	TruthEngine::Core::Entity Scene::AddEntity(const char* entityTag, const float4x4& tranform)
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>(tranform);
		entity.AddComponent<TagComponent>(entityTag);
		m_EntityTree.AddNode(entity);
		return entity;
	}

	std::vector<TruthEngine::Core::Entity> Scene::GetAncestor(const Entity& entity)
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

	std::vector<TruthEngine::Core::Entity> Scene::GetAncestor(entt::entity entityHandler)
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
		auto& t = entity.GetComponent<TransformComponent>().GetTransform();
		auto itr = m_EntityTree.m_Tree.find(entity);
		return t * GetParentTransforms(itr->second.mParent);
	}

	float4x4 Scene::CalcTransformsToRoot(entt::entity entityHandler)
	{
		auto& t = m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return t * GetParentTransforms(itr->second.mParent);
	}

	float4x4 Scene::GetParentTransforms(Entity parent)
	{
		if (!parent)
			return IdentityMatrix;

		auto& t = parent.GetComponent<TransformComponent>().GetTransform();

		return t * GetParentTransforms(m_EntityTree.m_Tree.find(parent)->second.mParent);
	}

	float4x4 Scene::GetParentTransforms(entt::entity parentHandler)
	{
		if (parentHandler == entt::null)
			return IdentityMatrix;

		auto& t = m_Registery.get<TransformComponent>(parentHandler).GetTransform();

		return t * GetParentTransforms(m_EntityTree.m_Tree.find(static_cast<uint32_t>(parentHandler))->second.mParent);
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

}