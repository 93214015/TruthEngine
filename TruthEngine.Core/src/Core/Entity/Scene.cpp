#include "pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

namespace TruthEngine::Core
{

	Scene::Scene() = default;

	Entity Scene::AddEntity()
	{
		Entity entity(this);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>();
		return entity;
	}

}