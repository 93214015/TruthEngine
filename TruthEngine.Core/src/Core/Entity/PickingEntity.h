#pragma once

namespace TruthEngine
{

	class Entity;
	class Scene;
	class Camera;

	class PickingEntity
	{
	public:

		static void PickEntity(float3 rayOrigin, float3 rayDirection, Scene* scene, Camera* camera);
		static void PickEntity(float2 mousePosition, Scene* scene, Camera* camera);
	};

}
