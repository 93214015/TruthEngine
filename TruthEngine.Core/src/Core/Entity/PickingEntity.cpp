#include "pch.h"
#include "PickingEntity.h"

#include "Core/Entity/Entity.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Components.h"
#include "Core/Entity/Model/Mesh.h"


#include "Core/Application.h"

using namespace DirectX;

void TruthEngine::Core::PickingEntity::PickEntity(float3 rayOrigin, float3 rayDirection, Scene* scene, Camera* camera)
{
}

void TruthEngine::Core::PickingEntity::PickEntity(float2 mousePosition, Scene* scene, Camera* camera)
{
	const auto& _proj = camera->GetProjection();

	float vx = (2.0f * mousePosition.x / TE_INSTANCE_APPLICATION->GetSceneViewportWidth() - 1.0f) / _proj(0, 0);
	float vy = (-2.0f * mousePosition.y / TE_INSTANCE_APPLICATION->GetSceneViewportHeight() + 1.0f) / _proj(1, 1);


	auto& cameraPosition = camera->GetPosition();
	XMVECTOR rayOrigin = XMVectorSet(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
	XMVECTOR rayDirection = XMVectorSet(vx, vy, 1.0f, 0.0f);

	const float4x4& _view = camera->GetView();
	XMMATRIX _viewInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_view));

	rayDirection = XMVector3TransformNormal(rayDirection, _viewInv);

	auto _entityGroup = scene->GroupEntities<BoundingBoxComponent, MeshComponent>();

	float _closestMeshDistance = FLT_MAX;

	for (auto entity : _entityGroup)
	{
		auto tag = scene->GetComponent<TagComponent>(entity).GetTag().c_str();

		const float4x4 _world = scene->CalcTransformsToRoot(entity);

		XMMATRIX _worldInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_world));

		auto _rayOrigin = XMVector3TransformCoord(rayOrigin, _worldInv);
		auto _rayDirection = XMVector3TransformNormal(rayDirection, _worldInv);
		
		_rayDirection = XMVector3Normalize(_rayDirection);

		const auto& _aabb = scene->GetComponent<BoundingBoxComponent>(entity).GetBoundingBox();

		float _aabbDistance = 0.0f;
		if (_aabb.Intersects(_rayOrigin, _rayDirection, _aabbDistance))
		{
			Mesh* mesh = scene->GetComponent<MeshComponent>(entity).GetMesh();

			const auto& _verteciesData = mesh->GetVertexBuffer()->GetVertexData<VertexData::Pos>();
			const auto& _indeciesData = mesh->GetIndexBuffer()->GetIndecies();

			const uint32_t _triNum = mesh->GetIndexNum() / 3;
			const uint32_t _indexOffset = mesh->GetIndexOffset();
			const uint32_t _vertexOffset = mesh->GetVertexOffset();

			for (uint32_t triIndex = 0; triIndex < _triNum; ++triIndex)
			{
				uint32_t _iindex = _indexOffset + (triIndex * 3);

				uint32_t i0 = _indeciesData[_iindex];
				uint32_t i1 = _indeciesData[_iindex + 1];
				uint32_t i2 = _indeciesData[_iindex + 2];

				XMVECTOR v0 = XMLoadFloat3(&_verteciesData[i0 + _vertexOffset].Position);
				XMVECTOR v1 = XMLoadFloat3(&_verteciesData[i1 + _vertexOffset].Position);
				XMVECTOR v2 = XMLoadFloat3(&_verteciesData[i2 + _vertexOffset].Position);

				float d = 0.0f;
				if (TriangleTests::Intersects(_rayOrigin, _rayDirection, v0, v1, v2, d))
				{
					if (d < _closestMeshDistance)
					{
						_closestMeshDistance = d;
						
						scene->SelectEntity(entity);
					}
				}

			}

		}


	}

}

void TruthEngine::Core::PickingEntity::PickEntity2(float2 mousePosition, uint32_t viewportWidth, uint32_t viewportHeight, Scene* scene, Camera* camera)
{
	auto& _proj = camera->GetProjection();


	float2 mousePosNDC{ (mousePosition.x / viewportWidth) * 2.0f - 1.0f , (mousePosition.y / viewportHeight) * -2.0f + 1.0f };


	float3 mousePosView{ mousePosNDC.x / _proj._11 , mousePosNDC.y / _proj._22, camera->GetZNearPlane() };
	

	float3 rayDirectionView{ .0f, .0f, 1.0f };

	auto& _view = camera->GetView();

	auto _XMView = XMLoadFloat4x4(&_view);
	XMVECTOR determinant = XMMatrixDeterminant(_XMView);
	auto _XMViewInv = XMMatrixInverse(&determinant, _XMView);

	auto _XMRayOriginWorld = XMVector3TransformCoord(XMLoadFloat3(&mousePosView), _XMViewInv);
	auto _XMRayDirectionWorld = XMVector3TransformNormal(XMLoadFloat3(&rayDirectionView), _XMViewInv);
	_XMRayDirectionWorld = XMVector3Normalize(_XMRayDirectionWorld);

	auto _entityGroup = scene->GroupEntities<BoundingBoxComponent, MeshComponent>();

	float _distMesh = FLT_MAX;

	for (auto _entity : _entityGroup)
	{
		const auto& _entityTranform = scene->GetComponent<TransformComponent>(_entity).GetTransform();

		auto _XMEntityTransform = XMLoadFloat4x4(&_entityTranform);
		auto _ETDeterminant = XMMatrixDeterminant(_XMEntityTransform);
		auto _entityTransformInv = XMMatrixInverse(&_ETDeterminant, _XMEntityTransform);

		auto _XMRayOriginLocal = XMVector3TransformCoord(_XMRayOriginWorld, _entityTransformInv);
		auto _XMRayDirectionLocal = XMVector3TransformNormal(_XMRayDirectionWorld, _entityTransformInv);
		_XMRayDirectionLocal = XMVector3Normalize(_XMRayDirectionLocal);

		auto _aabb = scene->GetComponent<BoundingBoxComponent>(_entity).GetBoundingBox();
		BoundingBox _aabbWorld;
		_aabb.Transform(_aabbWorld, _XMEntityTransform);

		float _distAABBWorld = 0.0f, _distAABBLocal = 0.0f;
		bool _intersectedLocal, _intersectedWorld;

		_intersectedLocal = _aabb.Intersects(_XMRayOriginLocal, _XMRayDirectionLocal, _distAABBLocal);
		_intersectedWorld = _aabbWorld.Intersects(_XMRayOriginWorld, _XMRayDirectionWorld, _distAABBWorld);
		
		if (_intersectedLocal)
		{
			if (!_intersectedWorld)
			{
				TE_LOG_CORE_WARN("MousePicking: the local space AABB was intersected but world space one not!");
			}
		}
		if (_intersectedWorld)
		{
			if (!_intersectedLocal)
			{
				TE_LOG_CORE_WARN("MousePicking: the World space AABB was intersected but local space one not!");
			}
		}

		if(_intersectedWorld || _intersectedLocal)
		{
			auto _mesh = scene->GetComponent<MeshComponent>(_entity).GetMesh();
			auto _indexOffset = _mesh->GetIndexOffset();
			auto _indexNum = _mesh->GetIndexNum();
			auto _vertexOffset = _mesh->GetVertexOffset();
			uint32_t _triangleNum = _indexNum / 3;

			auto& _vertexData = _mesh->GetVertexBuffer()->GetVertexData<VertexData::Pos>();
			auto& _indexData = _mesh->GetIndexBuffer()->GetIndecies();

			for (uint32_t _triangleIndex = 0; _triangleIndex < _triangleNum; ++_triangleIndex )
			{
				uint32_t _iindex = _indexOffset + (_triangleIndex * 3);

				uint32_t i0 = _indexData[_iindex];
				uint32_t i1 = _indexData[_iindex + 1];
				uint32_t i2 = _indexData[_iindex + 2];


				auto v0 = XMLoadFloat3(&_vertexData[i0 + _vertexOffset].Position);
				auto v1 = XMLoadFloat3(&_vertexData[i1 + _vertexOffset].Position);
				auto v2 = XMLoadFloat3(&_vertexData[i2 + _vertexOffset].Position);

				float _distTriangle;
				if (TriangleTests::Intersects(_XMRayOriginLocal, _XMRayDirectionLocal, v0, v1, v2, _distTriangle))
				{
					if (_distTriangle < _distMesh)
					{
						_distMesh = _distTriangle;

						scene->SelectEntity(_entity);
					}
				}

			}

		}
		

	}

}
