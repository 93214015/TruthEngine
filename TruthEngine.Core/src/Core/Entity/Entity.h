#pragma once

namespace TruthEngine
{
	namespace Core
	{
		class Scene;

		class Entity
		{
		public:
			Entity();
			Entity(Scene* scene);
			Entity(Scene* scene, entt::entity entityHandle);

			template<class T, class... Args>
			T& AddComponent(Args&&... args)
			{
				return m_Registery->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			}

			template<class T>
			T& GetComponent()
			{
				return m_Registery->get<T>(m_EntityHandle);
			}

			template<class T>
			bool HasComponent()
			{
				return m_Registery->has<T>(m_EntityHandle);
			}

			template<class T>
			void RemoveComponent()
			{
				m_Registery->remove<T>(m_EntityHandle);
			}

			operator bool()
			{
				return m_EntityHandle != entt::null;
			}

			operator entt::entity()
			{
				return m_EntityHandle;
			}

			bool operator==(const Entity& ent)
			{
				return m_EntityHandle == ent.m_EntityHandle;
			}

			bool operator==(const entt::entity & ent)
			{
				return m_EntityHandle == ent;
			}

			bool operator!=(const Entity& ent)
			{
				return m_EntityHandle != ent.m_EntityHandle;
			}

			operator uint32_t()
			{
				return static_cast<uint32_t>(m_EntityHandle);
			}

			operator uint32_t() const
			{
				return static_cast<uint32_t>(m_EntityHandle);
			}


		private:



		private:
			Scene* m_Scene;
			entt::registry* m_Registery;
			entt::entity m_EntityHandle;
		};

	}
}
