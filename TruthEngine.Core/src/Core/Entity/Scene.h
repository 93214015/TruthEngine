#pragma once

#include "Entity.h"

namespace TruthEngine
{
	namespace Core
	{
		class Scene
		{
		public:
			Scene();

			Entity AddEntity();

			template<class... Ts>
			auto& GroupEntities()
			{
				return m_Registery.group<Ts...>();
			}

			template<class T>
			T& GetComponent(entt::entity entityHandler)
			{
				return m_Registery.get<T>(entityHandler);
			}

			entt::registry& GetEntityRegistery()
			{
				return m_Registery;
			}

		private:

		private:
			entt::registry m_Registery;

			std::unordered_map<const char*, Entity> m_Entites;

			friend class Entity;
		};

		

	}
}
