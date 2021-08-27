#include "pch.h"
#include "Scene.h"


#include "Core/Application.h"

#include "Core/Event/EventKey.h"

#include "Core/Entity/Components.h"
#include "Core/Entity/Components/TranslationComponent.h"
#include "Core/Entity/Components/RotationComponent.h"
#include "Core/Entity/Components/LeaderComponent.h"
#include "Core/Entity/Components/AttachedComponent.h"
#include "Core/Entity/Model/ModelManager.h" // Mesh and Material Included
#include "Core/Entity/Model/AssimpLib.h"
#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Camera/CameraManager.h"

#include "Core/PhysicEngine/PhysicsEngine.h"

namespace TruthEngine
{

	Scene::Scene()
		: /*m_EntityTree(this),*/ m_LightManager(LightManager::GetInstace())
	{
	}

	void Scene::Init()
	{
		m_ModelShootedBall = AddEntity("ShootedBallM");
		RegisterEventListener();
	}

	Entity Scene::AddEntity(const char* _EntityTag, const float4A& _Translation, const float4A& _RotationQuaterion, const Entity* _ParentEntity)
	{
		Entity entity(m_Registery.create());


		//float4x4A _NewTransform = _Transform;
		float4A& _NewTranslation = AddComponent<TranslationComponent>(entity, _Translation).Translation;

		if (m_SelectedEntity)
		{
			if (HasComponent<BoundingBoxComponent>(m_SelectedEntity))
			{
				const BoundingAABox& _AABB = GetComponent<BoundingBoxComponent>(m_SelectedEntity).GetBoundingBox();
				const float4x4A& _transformOffset = GetComponent<TransformComponent>(m_SelectedEntity).GetTransform();
				/*_NewTransform._41 += _transformOffset._41;
				_NewTransform._42 += _transformOffset._42 + (_AABB.Extents.y + 10.0f);
				_NewTransform._43 += _transformOffset._43;*/
				_NewTranslation.x += _transformOffset._41;
				_NewTranslation.y += _transformOffset._42 + (_AABB.Extents.y + 10.0f);
				_NewTranslation.z += _transformOffset._43;

				float3 _v{ 0.0f, 1.0f, 0.0f };

				_v = Math::TransformVector(_v, _transformOffset);

				/*_NewTransform._41 += _v.x;
				_NewTransform._42 += _v.y;
				_NewTransform._43 += _v.z;*/

				_NewTranslation.x += _v.x;
				_NewTranslation.y += _v.y;
				_NewTranslation.z += _v.z;
			}
		}

		float4x4A _NewTransform = Math::TransformMatrix(Math::IdentityScale, _RotationQuaterion, _NewTranslation);

		AddComponent<RotationComponent>(entity, _RotationQuaterion);
		AddComponent<TransformComponent>(entity, _NewTransform /*, _WorldCenterOffset*/);
		AddComponent<TagComponent>(entity, _EntityTag);

		if (_ParentEntity != nullptr)
		{
			AddComponent<LeaderComponent>(entity, *_ParentEntity);
		}

		return entity;
	}

	Entity Scene::AddMeshEntity(const char* _MeshName, const float4A& _Translation, const float4A& _RotationQuaternion, const Mesh& _Mesh, Material* _Material, const Entity* _ParentEntity)
	{
		auto entity_mesh = AddEntity(_MeshName, _Translation, _RotationQuaternion);
		const Mesh& _NewMesh = AddComponent<MeshComponent>(entity_mesh, _Mesh).GetMesh();
		AddComponent<MaterialComponent>(entity_mesh, _Material);

		/*Add BoundignAABoxComponentand get the bounding box*/
		const BoundingAABox& _meshAABB = AddComponent<BoundingBoxComponent>(entity_mesh, _NewMesh).GetBoundingBox();

		//GetComponent<ModelComponent>(_ParentEntity).AddMeshEntity(entity_mesh);

		BoundingAABox _transformedAABB = _meshAABB;
		if (_Translation != Math::IdentityTranslate || _RotationQuaternion != Math::IdentityQuaternion)
		{
			XMMatrix _XMTransform = Math::XMTransformMatrix(Math::IdentityScale, _RotationQuaternion, _Translation);
			_meshAABB.Transform(_transformedAABB, _XMTransform);
		}

		/*Entity _Parent = *_ParentEntity;*/
		if (_ParentEntity != nullptr)
		{
			BoundingAABox& _ParentAABB = GetComponent<BoundingBoxComponent>(*_ParentEntity).GetBoundingBox();
			if (_ParentAABB.Extents == float3{ 1.0f, 1.0f, 1.0f })
			{
				_ParentAABB = _transformedAABB;
			}
			else
			{
				BoundingAABox::CreateMerged(_ParentAABB, _ParentAABB, _transformedAABB);
			}
		}

		if (m_BoundingBox.Extents == float3{ 1.0f, 1.0f, 1.0f })
		{
			m_BoundingBox = _transformedAABB;
		}
		else
		{
			BoundingAABox::CreateMerged(m_BoundingBox, m_BoundingBox, _transformedAABB);
		}

		return entity_mesh;
	}

	Entity Scene::AddPrimitiveMesh(const char* _MeshName, TE_PRIMITIVE_TYPE _PrimitiveType, const float3& _PrimitiveSize, const Entity* _ParentEntity)
	{
		const Mesh& _Mesh = TE_INSTANCE_MODELMANAGER->GeneratePrimitiveMesh(_PrimitiveType, _PrimitiveSize.x, _PrimitiveSize.y, _PrimitiveSize.z).GetMesh();
		Material* _Material = TE_INSTANCE_MATERIALMANAGER->AddDefaultMaterial(TE_IDX_MESH_TYPE::MESH_NTT);
		return AddMeshEntity(_MeshName, Math::IdentityTranslate, Math::IdentityQuaternion, _Mesh, _Material, _ParentEntity);
	}

	Entity Scene::AddEnvironmentEntity()
	{
		auto entity_environment = AddEntity("EnvironmentS");

		Mesh* _Mesh = nullptr;

		TE_INSTANCE_MODELMANAGER->GenerateEnvironmentMesh(&_Mesh);

		AddComponent<EnvironmentComponent>(entity_environment, *_Mesh);

		return entity_environment;
	}

	Entity Scene::AddLightEntity_Directional(
		const std::string_view _Name
		, const float3& _Position
		, const float _LightSize
		, const float3& _Strength
		, float _StrengthMultiplier
		, const float3& _Direction
		, const uint32_t _CastShadow
		, const float4& _CascadesCoveringDepth)
	{
		LightDirectional* _Light = m_LightManager->AddLightDirectional(
			_Name, _Position, _LightSize, _Strength, _StrengthMultiplier, _Direction, _CastShadow, _CascadesCoveringDepth
		);

		auto entityLight = AddEntity(_Name.data());
		AddComponent<LightComponent>(entityLight, _Light);

		auto& _Transform = GetComponent<TransformComponent>(entityLight).GetTransform();
		_Transform._41 = _Position.x;
		_Transform._42 = _Position.y;
		_Transform._43 = _Position.z;

		return entityLight;
	}

	Entity Scene::AddLightEntity_Spot(
		const std::string_view _Name
		, const float3& _Position
		, const float _LightSize
		, const float3& _Strength
		, float _StrengthMultiplier
		, const float3& _Direction
		, const bool _IsCastShadow
		, const float _FalloffStart
		, const float _FalloffEnd
		, const float _InnerConeAngle
		, const float _OuterConeAngle)
	{
		LightSpot* _Light = m_LightManager->AddLightSpot(
			_Name, _Position, _LightSize, _Strength, _StrengthMultiplier, _Direction, _IsCastShadow, _FalloffStart, _FalloffEnd, _InnerConeAngle, _OuterConeAngle
		);

		auto entityLight = AddEntity(_Name.data());
		AddComponent<LightComponent>(entityLight, _Light);

		auto& _Transform = GetComponent<TransformComponent>(entityLight).GetTransform();
		_Transform._41 = _Position.x;
		_Transform._42 = _Position.y;
		_Transform._43 = _Position.z;

		return entityLight;
	}

	Entity Scene::AddLightEntity_Point(
		const std::string_view _Name
		, const float3& _Position
		, const float _LightSize
		, const float3& _Strength
		, float _StrengthMultiplier
		, const bool _IsCastShadow
		, const float _AttenuationConstant
		, const float _AttenuationLinear
		, const float _AttenuationQuadrant)
	{
		LightPoint* _Light = m_LightManager->AddLightPoint(
			_Name, _Position, _LightSize, _Strength, _StrengthMultiplier, _IsCastShadow, _AttenuationConstant, _AttenuationLinear, _AttenuationQuadrant
		);

		auto entityLight = AddEntity(_Name.data());
		AddComponent<LightComponent>(entityLight, _Light);

		auto& _Transform = GetComponent<TransformComponent>(entityLight).GetTransform();
		_Transform._41 = _Position.x;
		_Transform._42 = _Position.y;
		_Transform._43 = _Position.z;

		return entityLight;
	}

	/*Entity Scene::AddModelEntity(const char* modelName, const float4x4A& _Transform)
	{
		//
		///Here we are trying to place new Mesh near the selected entity
		//
		float4x4A _Trans = _Transform;
		if (m_SelectedEntity)
		{
			if (HasComponent<BoundingBoxComponent>(m_SelectedEntity))
			{
				const BoundingAABox& _AABB = GetComponent<BoundingBoxComponent>(m_SelectedEntity).GetBoundingBox();
				const float4x4A& _transformOffset = GetComponent<TransformComponent>(m_SelectedEntity).GetTransform();
				_Trans._41 += _transformOffset._41;
				_Trans._42 += _transformOffset._42 + (_AABB.Extents.y + 20.0f);
				_Trans._43 += _transformOffset._43;

				float3 _v{ 0.0f, 1.0f, 0.0f };

				_v = Math::TransformVector(_v, _transformOffset);

				_Trans._41 += _v.x;
				_Trans._42 += _v.y;
				_Trans._43 += _v.z;
			}
		}


		Entity _ModelEntity = AddEntity(modelName, _Trans);
		_ModelEntity.AddComponent<ModelComponent>();


		BoundingAABox _AABB{};
		_ModelEntity.AddComponent<BoundingBoxComponent>(_AABB);


		return _ModelEntity;

	}*/

	/*
	std::vector<Entity> Scene::GetAncestor(const Entity entity)
	{
		std::vector<Entity> ancestors;

		auto itr = m_EntityTree.m_Tree.find(entity);
		auto parentEntity = itr->second.mParent;

		while (parentEntity)
		{

			ancestors.emplace_back(parentEntity);
			itr = m_EntityTree.m_Tree.find(parentEntity);
			parentEntity = itr->second.mParent;
		}

		return ancestors;
	}

	std::vector<Entity> Scene::GetAncestor(entt::entity entityHandler)
	{
		std::vector<Entity> ancestors;

		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		auto parentEntity = itr->second.mParent;

		while (parentEntity)
		{

			ancestors.emplace_back(parentEntity);
			itr = m_EntityTree.m_Tree.find(parentEntity);
			parentEntity = itr->second.mParent;
		}

		return ancestors;
	}

	float4x4A Scene::GetTransformHierarchy(Entity entity)
	{
		if (!entity)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		if (HasComponent<PhysicsDynamicComponent>(entity))
		{
			_Transform = &entity.GetComponent<PhysicsDynamicComponent>().GetTranform();
		}
		else
		{
			_Transform = &entity.GetComponent<TransformComponent>().GetTransform();
		}

		const auto itr = m_EntityTree.m_Tree.find(entity);
		return (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}

	float4x4A Scene::GetTransformHierarchy(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		if (m_Registery.has<PhysicsDynamicComponent>(entityHandler))
		{
			_Transform = &m_Registery.get<PhysicsDynamicComponent>(entityHandler).GetTranform();
		}
		else
		{
			_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		}


		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return  (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}

	float3 Scene::GetTranslateHierarchy(Entity entity)
	{
		float3 _Result = { .0f, .0f, .0f };

		if (!entity)
			return _Result;

		const float4x4A* _Transform = nullptr;

		if (HasComponent<PhysicsDynamicComponent>(entity))
		{
			_Transform = &GetComponent<PhysicsDynamicComponent>(entity).GetTranform();
		}
		else
		{
			_Transform = &GetComponent<TransformComponent>(entity).GetTransform();
		}

		_Result = _Result + float3{ _Transform->_41, _Transform->_42, _Transform->_43 };

		auto itr = m_EntityTree.m_Tree.find(entity);
		return  _Result + GetTranslateHierarchy(itr->second.mParent);
	}

	float3 Scene::GetTranslateHierarchy(entt::entity entityHandler)
	{
		float3 _Result = { .0f, .0f, .0f };

		if (entityHandler == entt::null)
			return _Result;

		const float4x4A* _Transform = nullptr;

		if (m_Registery.has<PhysicsDynamicComponent>(entityHandler))
		{
			_Transform = &m_Registery.get<PhysicsDynamicComponent>(entityHandler).GetTranform();
		}
		else
		{
			_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		}

		_Result = _Result + float3{ _Transform->_41, _Transform->_42, _Transform->_43 };

		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return  _Result + GetTranslateHierarchy(itr->second.mParent);
	}

	float4x4A Scene::GetStaticTransformHierarchy(Entity entity)
	{
		if (!entity)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		_Transform = &entity.GetComponent<TransformComponent>().GetTransform();

		const auto itr = m_EntityTree.m_Tree.find(entity);
		return (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}

	float4x4A Scene::GetStaticTransformHierarchy(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();


		auto itr = m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler));
		return  (*_Transform) * GetTransformHierarchy(itr->second.mParent);
	}
	*/


	const float4x4A& Scene::GetTransform(Entity _Entity) const
	{
		return GetComponent<TransformComponent>(_Entity).GetTransform();
	}


	void Scene::SetTransform(Entity _Entity, const float4x4A& _Transform)
	{
		GetComponent<TransformComponent>(_Entity).GetTransform() = _Transform;
	}


	void Scene::SetPosition(Entity _Entity, const float3A& _Translation)
	{
		float4x4A& _Transform = GetComponent<TransformComponent>(_Entity);

		_Transform._41 = _Translation.x;
		_Transform._42 = _Translation.y;
		_Transform._43 = _Translation.z;
	}

	void Scene::SetTranslationLocal(Entity _Entity, const float3A& _Translation)
	{
		SetTranslationLocal(_Entity, Math::ToXM(_Translation));
	}

	void Scene::SetTranslationRelative(Entity _Entity, const float3A& _Translation)
	{
		SetTranslationRelative(_Entity, Math::ToXM(_Translation));
	}

	void Scene::SetTranslationLocal(Entity _Entity, const XMVector& _XMTranslation)
	{
		float4x4A& _Transform = GetComponent<TransformComponent>(_Entity).GetTransform();

		XMMatrix _XMTransform = Math::ToXM(_Transform);

		const XMMatrix _XMNewTransform = Math::XMTransformMatrixTranslate(_XMTranslation);

		_XMTransform = Math::XMMultiply(_XMNewTransform, _XMTransform);

		_Transform = Math::FromXMA(_XMTransform);
	}

	void Scene::SetTranslationRelative(Entity _Entity, const XMVector& _XMTranslation)
	{
		float4x4A& _Transform = GetComponent<TransformComponent>(_Entity).GetTransform();

		const XMMatrix _XMNewTransform = Math::XMTransformMatrixTranslate(_XMTranslation);

		const XMMatrix _XMTransform = Math::XMMultiply(Math::ToXM(_Transform), _XMNewTransform);

		_Transform = Math::FromXMA(_XMTransform);
	}

	void Scene::SetPosition(Entity _Entity, const XMVector& _XMTranslation)
	{
		float4x4A& _Transform = GetComponent<TransformComponent>(_Entity);
		XMMatrix _XMTransform = Math::ToXM(_Transform);

		_XMTransform.r[3] = _XMTranslation;

		_Transform = Math::FromXMA(_XMTransform);
	}


	void Scene::SetRotation(Entity _Entity, const float4A& _Quaternion)
	{
		SetRotation(_Entity, Math::ToXM(_Quaternion));
	}

	void Scene::SetRotationLocal(Entity _Entity, const float4A& _Quaternion)
	{
		SetRotationLocal(_Entity, Math::ToXM(_Quaternion));
	}

	void Scene::SetRotationRelative(Entity _Entity, const float4A& _Quaternion)
	{
		SetRotationRelative(_Entity, Math::ToXM(_Quaternion));
	}

	void Scene::SetRotation(Entity _Entity, const XMVector& _XMQuaternionNew)
	{
		float4x4A& _Transform = GetComponent<TransformComponent>(_Entity).GetTransform();
		XMMatrix _XMTransform = Math::ToXM(_Transform);

		XMVector _XMTranslation, _XMScale, _XMQuaternion;
		Math::XMDecomposeMatrix(_XMTransform, _XMScale, _XMQuaternion, _XMTranslation);

		_XMTransform = Math::XMTransformMatrix(_XMScale, _XMQuaternionNew, _XMTranslation);

		_Transform = Math::FromXMA(_XMTransform);
	}

	void Scene::SetRotationLocal(Entity _Entity, const XMVector& _XMQuaternion)
	{
		float4x4A& _Transform = GetComponent<TransformComponent>(_Entity).GetTransform();

		XMMatrix _XMTransformRotation = Math::XMTransformMatrixRotation(_XMQuaternion);

		XMMatrix _XMTransform = Math::ToXM(_Transform);

		_XMTransform = Math::XMMultiply(_XMTransformRotation, _XMTransform);

		_Transform = Math::FromXMA(_XMTransform);
	}

	void Scene::SetRotationRelative(Entity _Entity, const XMVector& _XMQuaternion)
	{
		XMMatrix _XMTransformRotation = Math::XMTransformMatrixRotation(_XMQuaternion);

		float4x4A& _Transform = GetComponent<TransformComponent>(_Entity).GetTransform();
		XMMatrix _XMTransform = Math::ToXM(_Transform);

		_XMTransform = Math::XMMultiply(_XMTransform, _XMTransformRotation);

		_Transform = Math::FromXMA(_XMTransform);
	}




	const float4x4A& Scene::GetTransformHierarchy(Entity entity)
	{
		if (!entity)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		if (HasComponent<PhysicsDynamicComponent>(entity))
		{
			_Transform = &GetComponent<PhysicsDynamicComponent>(entity).GetTranform();
		}
		else
		{
			_Transform = &GetComponent<TransformComponent>(entity).GetTransform();
		}

		return *_Transform;
	}

	const float4x4A& Scene::GetTransformHierarchy(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		if (m_Registery.has<PhysicsDynamicComponent>(entityHandler))
		{
			_Transform = &m_Registery.get<PhysicsDynamicComponent>(entityHandler).GetTranform();
		}
		else
		{
			_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		}


		return *_Transform;
	}

	float3 Scene::GetTranslateHierarchy(Entity entity)
	{
		float3 _Result = { .0f, .0f, .0f };

		if (!entity)
			return _Result;

		const float4x4A* _Transform = nullptr;

		if (HasComponent<PhysicsDynamicComponent>(entity))
		{
			_Transform = &GetComponent<PhysicsDynamicComponent>(entity).GetTranform();
		}
		else
		{
			_Transform = &GetComponent<TransformComponent>(entity).GetTransform();
		}

		_Result = _Result + float3{ _Transform->_41, _Transform->_42, _Transform->_43 };

		return  _Result;
	}

	float3 Scene::GetTranslateHierarchy(entt::entity entityHandler)
	{
		float3 _Result = { .0f, .0f, .0f };

		if (entityHandler == entt::null)
			return _Result;

		const float4x4A* _Transform = nullptr;

		if (m_Registery.has<PhysicsDynamicComponent>(entityHandler))
		{
			_Transform = &m_Registery.get<PhysicsDynamicComponent>(entityHandler).GetTranform();
		}
		else
		{
			_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();
		}

		_Result = _Result + float3{ _Transform->_41, _Transform->_42, _Transform->_43 };


		return  _Result;
	}

	const float4x4A& Scene::GetStaticTransformHierarchy(Entity entity)
	{
		if (!entity)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		_Transform = &GetComponent<TransformComponent>(entity).GetTransform();

		return (*_Transform);
	}

	const float4x4A& Scene::GetStaticTransformHierarchy(entt::entity entityHandler)
	{
		if (entityHandler == entt::null)
			return IdentityMatrix;

		const float4x4A* _Transform = nullptr;

		_Transform = &m_Registery.get<TransformComponent>(entityHandler).GetTransform();

		return  (*_Transform);
	}

	const BoundingAABox& Scene::GetBoundingBox() const noexcept
	{

		return m_BoundingBox;
	}

	void Scene::UpdateBoundingBox(const BoundingAABox& _boundingBox)
	{
		/*if (m_BoundingBox.Contains(_boundingBox) == DirectX::CONTAINS)
			return;*/
		BoundingAABox::CreateMerged(m_BoundingBox, m_BoundingBox, _boundingBox);
	}



	/*
	bool Scene::HasParent(const Entity& entity) const
	{
		auto itr = m_EntityTree.m_Tree.find(entity);
		auto parentEntity = itr->second.mParent;
		return parentEntity;

		return HasComponent<LeaderComponent>(entity);
	}

	float4x4A Scene::GetParentTransforms(Entity parent)
	{
		if (!parent)
			return IdentityMatrix;

		auto& t = parent.GetComponent<TransformComponent>().GetTransform();

		return GetParentTransforms(m_EntityTree.m_Tree.find(parent)->second.mParent) * t;
	}

	float4x4A Scene::GetParentTransforms(entt::entity parentHandler)
	{
		if (parentHandler == entt::null)
			return IdentityMatrix;

		auto& t = m_Registery.get<TransformComponent>(parentHandler).GetTransform();

		return GetParentTransforms(m_EntityTree.m_Tree.find(static_cast<uint32_t>(parentHandler))->second.mParent) * t;
	}

	Entity Scene::GetModelEntity(Entity _Entity)
	{
		if (HasComponent<ModelComponent>(_Entity))
		{
			return _Entity;
		}
		else
		{
			return *GetParent(_Entity);
		}
	}

	Entity* Scene::GetParent(Entity _Entity)
	{

		const auto _Itr = m_EntityTree.m_Tree.find(_Entity);
		if (_Itr != m_EntityTree.m_Tree.cend())
		{
			return _Itr->second.mParent;
		}

		if (HasComponent<LeaderComponent>(_Entity))
		{
			return &(GetComponent<LeaderComponent>(_Entity).m_Entity);
		}

		return nullptr;
	}

	void Scene::AttachEntity(Entity _Parent, Entity _Attached)
	{
		AttachedComponent* _AttachedComponent = nullptr;
		if (HasComponent<AttachedComponent>(_Parent))
		{
			_AttachedComponent = &GetComponent<AttachedComponent>(_Parent);
		}
		else
		{
			_AttachedComponent = &AddComponent<AttachedComponent>(_Parent);
		}

		_AttachedComponent->AttachedEntities.push_back(_Attached);
	}

	void Scene::DetachEntity(Entity _Parent, Entity _Detached)
	{

		if (!HasComponent<AttachedComponent>(_Parent))
		{
			return;
		}

		auto& _AttachedEntites = GetComponent<AttachedComponent>(_Parent).AttachedEntities;

		std::remove_if(_AttachedEntites.begin(), _AttachedEntites.end(), [_Detached](Entity _Entity) { return _Entity == _Detached; });

		if (_AttachedEntites.size() == 0)
		{
			RemoveComponent<AttachedComponent>(_Parent);
		}

	}

	*/

	/*std::vector<Entity> Scene::GetChildrenEntity(Entity entity)
	{
		std::vector<Entity> r;

		for (auto& node : m_EntityTree.m_Tree.find(entity)->second.mChildren)
		{
			r.emplace_back(node.mEntity);
		}

		return r;
	}*/

	/*std::vector<Entity> Scene::GetChildrenEntity(entt::entity entityHandler)
	{
		std::vector<Entity> r;

		for (auto& node : m_EntityTree.m_Tree.find(static_cast<uint32_t>(entityHandler))->second.mChildren)
		{
			r.emplace_back(node.mEntity);
		}

		return r;
	}*/

	Entity Scene::CopyMeshEntity(Entity meshEntity)
	{
		static uint32_t s_copyIndex = 0;

		std::string name(meshEntity.GetComponent<TagComponent>().GetTag());

		/*if (name.find("_Copy") != std::string::npos)
		{
			name = name + std::to_string(s_copyIndex);
		}
		else
		{
			name = name + "_Copy" + std::to_string(s_copyIndex);
		}*/

		name = name + std::to_string(s_copyIndex);

		const TransformComponent& _TransformComponent = GetComponent<TransformComponent>(meshEntity);
		const float4x4A& transform = _TransformComponent.GetTransform();

		const Mesh& mesh = GetComponent<MeshComponent>(meshEntity).GetMesh();
		//auto newMesh = TE_INSTANCE_MODELMANAGER->CopyMesh(mesh);
		auto material = GetComponent<MaterialComponent>(meshEntity).GetMaterial();
		auto newMaterial = TE_INSTANCE_MATERIALMANAGER->AddMaterial(material);

		/*Entity* _ParentEntity = GetParent(meshEntity);*/

		const float4A& _Translation = GetComponent<TranslationComponent>(meshEntity).Translation;
		const float4A& _RotationQuaternion = GetComponent<RotationComponent>(meshEntity).Quaterion;

		auto _newMeshEntity = AddMeshEntity(name.c_str(), _Translation, _RotationQuaternion, mesh, newMaterial/*, _ParentEntity*/);

		SelectEntity(_newMeshEntity);

		s_copyIndex++;

		return _newMeshEntity;
	}

	void Scene::ImportModel(const char* filePath, Entity* _ParentEntity)
	{
		std::vector<ImportedMeshMaterials> _ImportedData = AssimpLib::GetInstance()->ImportModel(filePath);

		//ModelComponent& _ModelComponent = GetComponent<ModelComponent>(_ParentEntity);
		BoundingAABox* _ParentAABB = nullptr;
		if (_ParentEntity != nullptr)
		{
			if (HasComponent<BoundingBoxComponent>(*_ParentEntity))
				_ParentAABB = &GetComponent<BoundingBoxComponent>(*_ParentEntity).GetBoundingBox();
		}

		TE_INSTANCE_MODELMANAGER->InitVertexAndIndexBuffer();

		for (ImportedMeshMaterials& _Data : _ImportedData)
		{
			//const float3 _WorldCenterOffset = _MeshAABB.Center;
			Entity _MeshEntity = AddMeshEntity(_Data.mName.c_str(), Math::IdentityTranslate, Math::IdentityQuaternion, _Data.mMesh.GetMesh(), _Data.mMaterial, _ParentEntity);
			const BoundingAABox& _MeshAABB = GetComponent<BoundingBoxComponent>(_MeshEntity).GetBoundingBox();

			if (_Data.mAnimation)
			{
				AddComponent<SkinnedAnimationComponent>(_MeshEntity, _Data.mAnimation);
			}

			if (_ParentAABB != nullptr)
			{
				if (_ParentAABB->Extents == float3{ 1.0f, 1.0f, 1.0f })
				{
					*_ParentAABB = _MeshAABB;
				}
				else
				{
					BoundingAABox::CreateMerged(*_ParentAABB, *_ParentAABB, _MeshAABB);
				}
			}
		}

	}

	/*
	float3 Scene::GetPosition(Entity entity)
	{
		float3 _worldPostition = { 0.0f, 0.0f, 0.0f };
		if (HasComponent<BoundingBoxComponent>(entity))
		{
			const auto& _aabb = GetComponent<BoundingBoxComponent>(entity).GetBoundingBox();

			_worldPostition.x += _aabb.Center.x;
			_worldPostition.y += _aabb.Center.y;
			_worldPostition.z += _aabb.Center.z;
		}

		const auto& _transform = GetTransformHierarchy(entity);
		auto xmPos = XMVector3TransformCoord(XMLoadFloat3(&_worldPostition), XMLoadFloat4x4(&_transform));
		XMStoreFloat3(&_worldPostition, xmPos);

		return  _worldPostition;
	}

	float3 Scene::GetPosition(entt::entity entityHandle)
	{
		const auto& _transform = GetTransformHierarchy(entityHandle);
		float3 _worldPostition = { 0.0f, 0.0f, 0.0f };
		if (HasComponent<BoundingBoxComponent>(entityHandle))
		{
			const auto& _aabb = GetComponent<BoundingBoxComponent>(entityHandle).GetBoundingBox();

			_worldPostition.x += _aabb.Center.x;
			_worldPostition.y += _aabb.Center.y;
			_worldPostition.z += _aabb.Center.z;
		}

		auto xmPos = XMVector3TransformCoord(XMLoadFloat3(&_worldPostition), XMLoadFloat4x4(&_transform));
		XMStoreFloat3(&_worldPostition, xmPos);

		return  _worldPostition;
	}
	*/

	Camera* Scene::GetActiveCamera() const
	{
		return CameraManager::GetInstance()->GetActiveCamera();
	}

	void Scene::ShootTheBall()
	{
		static uint32_t _BallIndex = 0;
		std::string _BallName = "Ball" + std::to_string(_BallIndex);
		Entity _BallEntity = AddPrimitiveMesh(_BallName.c_str(), TE_PRIMITIVE_TYPE::SPHERE, float3{ 1.0f, 0.0f, 0.0f }, nullptr /*, &m_ModelShootedBall*/);

		Camera* _ActiveCamera = GetActiveCamera();
		float3 _LinearVelocity = _ActiveCamera->GetLook() * float3 { 120.0f, 120.0f, 120.0f };

		float4x4A _Transform = Math::TransformMatrixTranslate(_ActiveCamera->GetPosition());

		TEPhysicsRigidDesc _RigidDesc{ 0.5f, 0.5f, 0.5f, _Transform };
		TEPhysicsRigidSphereDesc _SphereRigidDesc{ 1.0f, _RigidDesc };

		TE_INSTANCE_PHYSICSENGINE->AddRigidDynamicSphere(_SphereRigidDesc, _BallEntity, _LinearVelocity);
		TE_INSTANCE_PHYSICSENGINE->Play();
	}

	void Scene::RegisterEventListener()
	{
		auto _LambdaOnKeyReleased = [this](Event& _Event)
		{
			OnEventKeyPressed(static_cast<EventKeyReleased&>(_Event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::KeyReleased, _LambdaOnKeyReleased);
	}

	void Scene::OnEventKeyPressed(EventKeyReleased& _event)
	{
		if (_event.GetKeyCode() == Key::K)
		{
			ShootTheBall();
		}
	}

	void Scene::OnUpdate(double DeltaTime)
	{
		m_SystemMovement.OnUpdate(this, DeltaTime);
		m_SystemUpdateTransforms.OnUpdate(this, DeltaTime);
		m_SystemDynamicLights.OnUpdate(this, DeltaTime);
	}


	//
	// Statics
	//
	Scene* Scene::s_ActiveScene = nullptr;

}