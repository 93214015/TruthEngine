#pragma once

#include "Entity.h"

namespace TruthEngine::Core
{
	struct EntityNode
	{
		EntityNode() = default;
		EntityNode(Entity entity, Entity parent)
			: mEntity(entity), mParent(parent)
		{
			mChildren.reserve(5);
		}

		Entity mEntity;
		Entity mParent;
		std::vector<EntityNode> mChildren;
	};

	class EntityTree
	{
	public:
		EntityTree(Scene* scene);
		void AddNode(Entity entity, Entity parent);
		void AddNode(Entity entity);

	private:
		std::unordered_map<uint32_t, EntityNode> m_Tree;
		EntityNode m_Root;
		Scene* m_Scene;

		//
		//Friend Class
		//
		friend class Scene;
	};
}
