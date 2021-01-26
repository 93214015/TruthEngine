#pragma once

namespace TruthEngine::Core
{

	class Entity;
	class Scene;
	class Camera;

	class PickingEntity
	{
	public:

		static void PickEntity(float3 rayOrigin, float3 rayDirection, Scene* scene, Camera* camera);
		static void PickEntity(float2 mousePosition, Scene* scene, Camera* camera);
		static void PickEntity2(float2 mousePosition, uint32_t viewportWidth, uint32_t viewportHeight, Scene* scene, Camera* camera);
	};

}
