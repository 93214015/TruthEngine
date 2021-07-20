#pragma once

#include "EntityTree.h"

#include "Core/Entity/Model/ModelManager.h"
#include "Core/Renderer/MaterialManager.h"

namespace TruthEngine
{

	class EventKeyReleased;

	class LightManager;
	class CameraManager;
	class Camera;

	class Material;
	class Mesh;

	class ModelComponent;

	class SA_Animation;

	class Scene
	{
	public:
		Scene();

		void Init();

		Entity AddEntity(const char* entityTag, Entity parent, const float4x4A& transform = IdentityMatrix /*, const float3& _WorldCenterOffset = float3{ .0f, .0f,.0f }*/);
		Entity AddEntity(const char* entityTag, const float4x4A& transform = IdentityMatrix /*, const float3& _WorldCenterOffset = float3{ .0f, .0f,.0f }*/);

		Entity AddMeshEntity(const char* _MeshName, const float4x4A& _Transform, const Mesh& _Mesh, Material* _Material, Entity _ModelEntity);
		Entity AddPrimitiveMesh(const char* _MeshName, TE_PRIMITIVE_TYPE _PrimitiveType, const float3& _PrimitiveSize, Entity _ModelEntity);
		Entity AddEnvironmentEntity();

		Entity AddLightEntity_Directional(
			const std::string_view _Name
			, const float3& _Strength
			, const float3& _Direction
			, const float3& _Position
			, const float _LightSize
			, const uint32_t _CastShadow
			, const float4& _CascadesCoveringDepth
		);

		Entity AddLightEntity_Spot(
			const std::string_view _Name,
			const float3& _Strength,
			const float3& _Direction,
			const float3& _Position,
			const float _LightSize,
			const bool _IsCastShadow,
			const float _FalloffStart,
			const float _FalloffEnd,
			const float _InnerConeAngle,
			const float _OuterConeAngle
		);

		Entity AddModelEntity(const char* modelName, const float4x4A& transform);

		Entity CopyMeshEntity(Entity meshsEntity);

		void ImportModel(const char* filePath, std::string _ModelName);

		template<class... Ts>
		auto GroupEntities()
		{
			std::scoped_lock<std::mutex> lock(m_Mutex);
			return m_Registery.group<Ts...>();
		}

		template<class... Ts>
		auto ViewEntities()
		{
			std::scoped_lock<std::mutex> lock(m_Mutex);
			return m_Registery.view<Ts...>();
		}

		template<class T>
		T& GetComponent(entt::entity entityHandler)
		{
			std::scoped_lock<std::mutex> lock(m_Mutex);
			return m_Registery.get<T>(entityHandler);
		}

		template<typename T>
		bool HasComponent(entt::entity entityHandler)
		{
			std::scoped_lock<std::mutex> lock(m_Mutex);
			return m_Registery.has<T>(entityHandler);
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
			m_SelectedEntity = Entity(this, entityHandle);
		}

		inline void ClearSelectedEntity()
		{
			m_SelectedEntity = {};
		}

		Entity GetModelEntity(Entity _Entity);
		Entity GetParent(const Entity _Entity) const;
		std::vector<Entity> GetChildrenEntity(Entity entity);
		std::vector<Entity> GetChildrenEntity(entt::entity entityHandler);

		inline std::vector<EntityNode>& GetChildrenNodes(Entity entity)
		{
			return m_EntityTree.m_Tree.find(entity)->second.mChildren;
		}
		inline std::vector<EntityNode>& GetChildrenNodes(entt::entity entityHandler)
		{
			return m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler))->second.mChildren;
		}

		std::vector<Entity> GetAncestor(const Entity entity);
		std::vector<Entity> GetAncestor(entt::entity entityHandler);

		float4x4A GetTransformHierarchy(Entity entity);
		float4x4A GetTransformHierarchy(entt::entity entityHandler);

		float3 GetTranslateHierarchy(Entity entity);
		float3 GetTranslateHierarchy(entt::entity entityHandler);

		float4x4A GetStaticTransformHierarchy(Entity entity);
		float4x4A GetStaticTransformHierarchy(entt::entity entityHandler);

		const BoundingAABox& GetBoundingBox() const noexcept;
		void UpdateBoundingBox(const BoundingAABox& _boundingBox);

		inline bool HasParent(const Entity& entity)
		{
			auto itr = m_EntityTree.m_Tree.find(entity);
			auto parentEntity = itr->second.mParent;
			return parentEntity;
		}

		float3 GetPosition(Entity entity);
		float3 GetPosition(entt::entity entityHandle);

		Camera* GetActiveCamera() const;

		void ShootTheBall();

		void RegisterEventListener();
		void OnEventKeyPressed(EventKeyReleased& _event);
		

	private:
		float4x4A GetParentTransforms(Entity parent);
		float4x4A GetParentTransforms(entt::entity parentHandler);

	private:
		entt::registry m_Registery;

		EntityTree m_EntityTree;
		Entity m_SelectedEntity;

		Entity m_ModelShootedBall;

		std::unordered_map<const char*, Entity> m_Entites;

		BoundingAABox m_BoundingBox;

		std::mutex m_Mutex;

		LightManager* m_LightManager;

		//
		//Friend Classes
		//
		friend class Entity;
		/*friend class EntityTree;*/
	};
}
