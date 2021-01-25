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

			for (uint32_t triIndex = 0; triIndex < _triNum; ++triIndex)
			{
				uint32_t _iindex = _indexOffset + (triIndex * 3);

				uint32_t i0 = _indeciesData[_iindex];
				uint32_t i1 = _indeciesData[_iindex + 1];
				uint32_t i2 = _indeciesData[_iindex + 2];

				XMVECTOR v0 = XMLoadFloat3(&_verteciesData[i0].Position);
				XMVECTOR v1 = XMLoadFloat3(&_verteciesData[i1].Position);
				XMVECTOR v2 = XMLoadFloat3(&_verteciesData[i2].Position);

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
