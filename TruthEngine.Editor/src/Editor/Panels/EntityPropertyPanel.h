#pragma once

#include "Core/Entity/Scene.h"
#include "Core/Entity/Entity.h"


namespace TruthEngine
{
	//Forward Declaration
	
		class Entity;
		
		class TransformComponent;
		class MaterialComponent;
		class CameraComponent;
		class LightComponent;
		class TagComponent;

		enum class TE_PHYSICS_RIGID_TYPE;
		enum class TE_PHYSICS_RIGID_SHAPE;

		class TEPhysicsRigidDesc;

	class EntityPropertyPanel
	{
	public:
		EntityPropertyPanel() = default;

		inline void SetContext(Entity context) { m_Context = context; }
		
		void OnImGuiRender();

	private:
		inline void DrawTagComponent(TagComponent& component);
		inline void DrawTransformComponent(TransformComponent& component);
		inline void DrawMaterialComponent(MaterialComponent& component);
		inline void DrawCameraComponent(CameraComponent& component);
		inline void DrawLightComponent(LightComponent& component);

		void AddPhysicsComponent(TE_PHYSICS_RIGID_TYPE rigidType, TE_PHYSICS_RIGID_SHAPE rigidshape, TEPhysicsRigidDesc& rigidDesc);
	private:
		Entity m_Context;

		void DrawImGuizmo();
		void DrawPhysicComponent();
	};
}
