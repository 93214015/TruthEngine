#include "pch.h"
#include "EntityTree.h"

#include "Scene.h"

namespace TruthEngine::Core
{
	EntityTree::EntityTree(Scene* scene)
		: m_Scene(scene), m_Root(EntityNode{ Entity(scene, scene->AddEntity("rootNode", Entity())), Entity() })
	{}

	void EntityTree::AddNode(Entity entity, Entity parent)
	{
		UINT io = 0;
		if (parent)
			io = 1;

		EntityNode node(entity, parent);

		//auto& result = m_Tree.emplace( std::pair(entity, node) );
		 m_Tree[static_cast<uint32_t>(entity)] = node;

		if (parent)
		{
			auto& parentNode = m_Tree.find(parent)->second;
			parentNode.mChildren.emplace_back(node);
		}
	}

	void EntityTree::AddNode(Entity entity)
	{
		EntityNode node(entity, m_Root.mEntity);

		m_Tree.emplace(entity, node);

		m_Root.mChildren.emplace_back(node);
	}

}