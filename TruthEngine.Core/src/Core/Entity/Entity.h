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

			template<class T, class... Args>
			T& AddComponent(Args&&... args)
			{
				return m_Scene->m_Registery.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			}

			template<class T>
			T& GetComponent()
			{
				return m_Scene->m_Registery.get<T>(m_EntityHandle);
			}

			template<class T>
			bool HasComponent()
			{
				return m_Scene->m_Registery.has<T>(m_EntityHandle);
			}

			template<class T>
			void RemoveComponent()
			{
				m_Scene->m_Registery.remove<T>(m_EntityHandle);
			}

			operator entt::entity()
			{
				return m_EntityHandle;
			}

		private:



		private:
			Scene* m_Scene;
			entt::entity m_EntityHandle;
		};

	}
}
