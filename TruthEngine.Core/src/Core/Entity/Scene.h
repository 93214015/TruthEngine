#pragma once

#include "EntityTree.h"

#include "Core/Entity/Model/ModelManager.h"
#include "Core/Renderer/MaterialManager.h"

#include "Core/Entity/Systems/SystemMovement.h"
#include "Core/Entity/Systems/SystemUpdateTransform.h"
#include "Core/Entity/Systems/SystemDynamicLights.h"

namespace TruthEngine
{

#pragma region Forward Declarations
	class EventKeyReleased;
	class LightManager;
	class CameraManager;
	class Camera;
	class Material;
	class Mesh;
	class ModelComponent;
	class SA_Animation;
#pragma endregion


	class Scene
	{
	public:
		Scene();

		void Init();

		Entity AddEntity(const char* _EntityTag, const float4A& _Translation = Math::IdentityTranslate, const float4A& RotationQuaterion = Math::IdentityQuaternion, const Entity* _ParentEntity = nullptr);

		Entity AddMeshEntity(const char* _MeshName, const float4A& _Translation, const float4A& _RotationQuaternion, const Mesh& _Mesh, Material* _Material, const Entity* _ParentEntity = nullptr);
		Entity AddPrimitiveMesh(const char* _MeshName, TE_PRIMITIVE_TYPE _PrimitiveType, const float3& _PrimitiveSize, float radius, const int3 slices, const int3& segments, const Entity* _ParentEntity = nullptr);
		Entity AddEnvironmentEntity();

		void SetMovement(Entity _Entity, const float3A& _Movement, bool _IsAbsolute);
		void SetDirection(Entity _Entity, const float3A& _Direction);
		void SetTransform(Entity _Entity, const float3A& _Direction, const float3A& _Position);

		Entity AddLightEntity_Directional(
			const std::string_view _Name
			, const float3& _Position
			, const float _LightSize
			, const float3& _Strength
			, float _StrengthMultiplier
			, const float3& _Direction
			, const uint32_t _CastShadow
			, const float4& _CascadesCoveringDepth
		);

		Entity AddLightEntity_Spot(
			const std::string_view _Name,
			const float3& _Position,
			const float _LightSize,
			const float3& _Strength,
			float StrengthMultiplier,
			const float3& _Direction,
			const bool _IsCastShadow,
			const float _FalloffStart,
			const float _FalloffEnd,
			const float _InnerConeAngle,
			const float _OuterConeAngle
		);

		Entity AddLightEntity_Point(
			const std::string_view _Name,
			const float3& _Position,
			const float _LightSize,
			const float3& _Strength,
			float StrengthMultiplier,
			const bool _IsCastShadow,
			float _AttenuationStartRadius,
			float _AttenuationEndRadius
		);


		//Entity AddModelEntity(const char* modelName, const float4x4A& transform);

		Entity CopyMeshEntity(Entity meshsEntity);

		void ImportModel(const char* filePath, Entity* _ParentEntity);

		template<class... Ts>
		auto GroupEntities()
		{
			return m_Registery.group<Ts...>();
		}

		template<class... Ts>
		auto ViewEntities()
		{
			return m_Registery.view<Ts...>();
		}

		template<class... Ts>
		auto ViewEntities() const
		{
			return m_Registery.view<Ts...>();
		}

		template<class T>
		T& GetComponent(entt::entity entityHandler)
		{
			return m_Registery.get<T>(entityHandler);
		}

		template<class T>
		T& GetOrAddComponent(Entity _Entity)
		{
			return m_Registery.get_or_emplace<T>(_Entity);
		}

		template<class T>
		const T& GetComponent(entt::entity entityHandler) const
		{
			return m_Registery.get<T>(entityHandler);
		}

		template<class T, typename... Args>
		inline T& AddComponent(Entity _Entity, Args&&... _Args)
		{
			return m_Registery.emplace<T>(_Entity.m_EntityHandle, std::forward<Args>(_Args)...);
		}

		template<class T, typename... Args>
		inline T& AddComponent(entt::entity _Entity, Args&&... _Args)
		{
			return m_Registery.emplace<T>(_Entity, std::forward<Args>(_Args)...);
		}

		template<class T, typename... Args>
		inline T& AddOrReplaceComponent(Entity _Entity, Args&&... _Args)
		{
			return m_Registery.emplace_or_replace<T>(_Entity.m_EntityHandle, std::forward<Args>(_Args)...);
		}

		template<class T, typename... Args>
		inline T& AddOrReplaceComponent(entt::entity _Entity, Args&&... _Args)
		{
			return m_Registery.emplace_or_replace<T>(_Entity, std::forward<Args>(_Args)...);
		}

		template<class T>
		inline void RemoveComponent(Entity _Entity)
		{
			m_Registery.remove<T>(_Entity.m_EntityHandle);
		}

		template<typename... ARGS>
		bool HasComponent(entt::entity entityHandler) const
		{
			return m_Registery.all_of<ARGS...>(entityHandler);
		}

		entt::registry& GetEntityRegistery()
		{
			return m_Registery;
		}

		inline Entity GetSelectedEntity() const
		{
			return m_SelectedEntity;
		}

		inline void SelectEntity(const Entity& entity)
		{
			m_SelectedEntity = entity;
		}

		inline void SelectEntity(entt::entity entityHandle)
		{
			m_SelectedEntity = Entity(entityHandle);
		}

		inline void ClearSelectedEntity()
		{
			m_SelectedEntity = {};
		}

		//Entity GetModelEntity(Entity _Entity);
		//Entity* GetParent(Entity _Entity);
		//std::vector<Entity> GetChildrenEntity(Entity entity);
		//std::vector<Entity> GetChildrenEntity(entt::entity entityHandler);

		/*inline std::vector<EntityNode>& GetChildrenNodes(Entity entity)
		{
			return m_EntityTree.m_Tree.find(entity)->second.mChildren;
		}
		inline std::vector<EntityNode>& GetChildrenNodes(entt::entity entityHandler)
		{
			return m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler))->second.mChildren;
		}

		std::vector<Entity> GetAncestor(const Entity entity);
		std::vector<Entity> GetAncestor(entt::entity entityHandler);

		void AttachEntity(Entity Parent, Entity Attached);
		void DetachEntity(Entity Parent, Entity Detached);
		*/

		const float4x4A& GetTransform(Entity _Entity) const;

		void SetTransform(Entity _Entity, const float4x4A& _Transform);

		void SetPosition(Entity _Entity, const float3A& _Translation);
		void SetPosition(Entity _Entity, const XMVector& _Translation);
		void SetTranslationLocal(Entity _Entity, const float3A& _Translation);
		void SetTranslationLocal(Entity _Entity, const XMVector& _Translation);
		void SetTranslationRelative(Entity _Entity, const float3A& _Translation);
		void SetTranslationRelative(Entity _Entity, const XMVector& _Translation);

		void SetRotation(Entity _Entity, const float4A& _Quaternion);
		void SetRotation(Entity _Entity, const XMVector& _Quaternion);
		void SetRotationLocal(Entity _Entity, const float4A& _Quaternion);
		void SetRotationLocal(Entity _Entity, const XMVector& _Quaternion);
		void SetRotationRelative(Entity _Entity, const float4A& _Quaternion);
		void SetRotationRelative(Entity _Entity, const XMVector& _Quaternion);

		const float4x4A& GetTransformHierarchy(Entity entity);
		const float4x4A& GetTransformHierarchy(entt::entity entityHandler);

		float3 GetTranslateHierarchy(Entity entity);
		float3 GetTranslateHierarchy(entt::entity entityHandler);

		const float4x4A& GetStaticTransformHierarchy(Entity entity);
		const float4x4A& GetStaticTransformHierarchy(entt::entity entityHandler);

		const BoundingAABox& GetBoundingBox() const noexcept;
		void UpdateBoundingBox(const BoundingAABox& _boundingBox);

		/*
		bool HasParent(const Entity& entity) const;

		float3 GetPosition(Entity entity);
		float3 GetPosition(entt::entity entityHandle);
		*/

		Camera* GetActiveCamera() const;

		void ShootTheBall();

		void RegisterEventListener();
		void OnEventKeyPressed(EventKeyReleased& _event);

		void OnUpdate(double DeltaTime);

		//
		//Static
		//
		static Scene* s_ActiveScene;

		static void SetActiveScene(Scene* _Scene)
		{
			s_ActiveScene = _Scene;
		}
		

	private:
		/*float4x4A GetParentTransforms(Entity parent);
		float4x4A GetParentTransforms(entt::entity parentHandler);*/

	private:
		entt::registry m_Registery;

		//EntityTree m_EntityTree;
		Entity m_SelectedEntity = {};

		Entity m_ModelShootedBall;

		std::unordered_map<const char*, Entity> m_Entites;

		BoundingAABox m_BoundingBox;

		//std::mutex m_Mutex;

		LightManager* m_LightManager;

		/*std::deque<Entity> m_UpdatedEntities;
		std::deque<Entity, const XMMatrix&> m_UpdateFollower;*/

		//
		//Systems
		//
		SystemMovement m_SystemMovement;
		SystemUpdateTransform m_SystemUpdateTransforms;
		SystemDynamicLights m_SystemDynamicLights;


		//
		// Statics
		//

		//
		//Friend Classes
		//
		friend class Entity;
		/*friend class EntityTree;*/
	};


	inline Scene* GetActiveScene()
	{
		return Scene::s_ActiveScene;
	}
}
