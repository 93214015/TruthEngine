#pragma once

#include "EntityTree.h"

namespace TruthEngine
{
	namespace Core
	{
		class Scene
		{
		public:
			Scene();

			Entity AddEntity(const char* entityTag, Entity parent, const float4x4& transform = IdentityMatrix);
			Entity AddEntity(const char* entityTag, const float4x4& transform = IdentityMatrix);

			template<class... Ts>
			auto GroupEntities()
			{
				return m_Registery.group<Ts...>();
			}

			template<class... Ts>
			auto ViewEntities()
			{
				return m_Registery.view<Ts...>();
			}

			template<class T>
			T& GetComponent(entt::entity entityHandler)
			{
				return m_Registery.get<T>(entityHandler);
			}

			template<typename T>
			bool HasComponent(entt::entity entityHandler)
			{
				return m_Registery.has<T>(entityHandler);
			}

			entt::registry& GetEntityRegistery()
			{
				return m_Registery;
			}

			inline Entity GetSelectedEntity() const
			{
				return m_SelectedEntity;
			}

			inline void SelectEntity(const Entity& entity)
			{
				m_SelectedEntity = entity;
			}

			inline void ClearSelectedEntity()
			{
				m_SelectedEntity = {};
			}

			std::vector<Entity> GetChildrenEntity(Entity entity);
			std::vector<Entity> GetChildrenEntity(entt::entity entityHandler);

			inline std::vector<EntityNode>& GetChildrenNodes(Entity entity)
			{
				return m_EntityTree.m_Tree.find(entity)->second.mChildren;
			}
			inline std::vector<EntityNode>& GetChildrenNodes(entt::entity entityHandler)
			{
				return m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler))->second.mChildren;
			}

			std::vector<Entity> GetAncestor(const Entity& entity);
			float4x4 CalcTransformsToRoot(Entity& entity);
			std::vector<Entity> GetAncestor(entt::entity entityHandler);
			float4x4 CalcTransformsToRoot(entt::entity entityHandler);



			inline bool HasParent(const Entity& entity)
			{
				auto itr = m_EntityTree.m_Tree.find(entity);
				auto parentEntity = itr->second.mParent;
				return parentEntity;
			}

		private:
			float4x4 GetParentTransforms(Entity parent);
			float4x4 GetParentTransforms(entt::entity parentHandler);
		private:
			entt::registry m_Registery;

			EntityTree m_EntityTree;
			Entity m_SelectedEntity;

			std::unordered_map<const char*, Entity> m_Entites;

			//
			//Friend Classes
			//
			friend class Entity;
			/*friend class EntityTree;*/
		};



	}
}
