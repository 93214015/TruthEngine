#pragma once

#include "Core/Entity/Scene.h"
#include "Core/Entity/Entity.h"


namespace TruthEngine
{
	//Forward Declaration
	namespace Core
	{
		class Entity;
		
		class TransformComponent;
		class MaterialComponent;
		class CameraComponent;
		class LightComponent;
		class TagComponent;

		enum class TE_PHYSICS_RIGID_TYPE;
		enum class TE_PHYSICS_RIGID_SHAPE;

		class TEPhysicsRigidDesc;
	}

	class EntityPropertyPanel
	{
	public:
		EntityPropertyPanel() = default;

		inline void SetContext(Core::Entity context) { m_Context = context; }
		
		void OnImGuiRender();

	private:
		inline void DrawTagComponent(Core::TagComponent& component);
		inline void DrawTransformComponent(Core::TransformComponent& component);
		inline void DrawMaterialComponent(Core::MaterialComponent& component);
		inline void DrawCameraComponent(Core::CameraComponent& component);
		inline void DrawLightComponent(Core::LightComponent& component);

		void AddPhysicsComponent(Core::TE_PHYSICS_RIGID_TYPE rigidType, Core::TE_PHYSICS_RIGID_SHAPE rigidshape, Core::TEPhysicsRigidDesc& rigidDesc);
	private:
		Core::Entity m_Context;

		void DrawImGuizmo();
		void DrawPhysicComponent();
	};
}
