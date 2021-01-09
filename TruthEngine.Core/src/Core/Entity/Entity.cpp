#include "pch.h"
#include "Entity.h"

#include "Scene.h"

namespace TruthEngine::Core
{
	Entity::Entity()
		: m_Scene(nullptr), m_EntityHandle(entt::null)
	{
	}

	Entity::Entity(Scene* scene)
		: m_Scene(scene), m_EntityHandle(scene->m_Registery.create())
	{
	}
}