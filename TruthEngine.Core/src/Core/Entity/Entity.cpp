#include "pch.h"
#include "Entity.h"

#include "Scene.h"

namespace TruthEngine::Core
{
	Entity::Entity()
		: m_Scene(nullptr), m_Registery(nullptr), m_EntityHandle(entt::null)
	{
	}

	Entity::Entity(Scene* scene)
		: m_Scene(scene), m_Registery(&scene->m_Registery), m_EntityHandle(scene->m_Registery.create())
	{
	}


	Entity::Entity(Scene* scene, entt::entity entityHandle)
		: m_Scene(scene), m_Registery(&scene->m_Registery), m_EntityHandle(entityHandle)
	{}


}