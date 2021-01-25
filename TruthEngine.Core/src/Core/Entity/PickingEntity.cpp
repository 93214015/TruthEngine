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

	float vx = (2.0f * mousePosition.x / TE_INSTANCE_APPLICATION->GetSceneViewportWidth() - 1.0f) / _proj._11;
	float vy = (-2.0f * mousePosition.y / TE_INSTANCE_APPLICATION->GetSceneViewportHeight() + 1.0f) / _proj._22;

	XMVECTOR rayOrigin = XMVectorSet(.0f, .0f, .0f, 1.0f);
	XMVECTOR rayDirection = XMVectorSet(vx, vy, 1.0f, 0.0f);

	const float4x4& _view = camera->GetView();
	XMMATRIX _viewInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_view));

	auto _entityGroup = scene->GroupEntities<BoundingBoxComponent, MeshComponent>();

	float _closestMeshDistance = FLT_MAX;

	for (auto entity : _entityGroup)
	{

		const float4x4 _world = scene->CalcTransformsToRoot(entity);

		XMMATRIX _worldInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_world));

		XMMATRIX _toLocal = XMMatrixMultiply(_viewInv, _worldInv);

		rayOrigin = XMVector3TransformCoord(rayOrigin, _toLocal);
		rayDirection = XMVector3TransformNormal(rayDirection, _toLocal);

		rayDirection = XMVector3NormalizeEst(rayDirection);

		const auto& _aabb = scene->GetComponent<BoundingBoxComponent>(entity).GetBoundingBox();

		float _aabbDistance = 0.0f;
		if (_aabb.Intersects(rayOrigin, rayDirection, _aabbDistance))
		{
			Mesh* mesh = scene->GetComponent<MeshComponent>(entity).GetMesh();

			const auto& _verteciesData = mesh->GetVertexBuffer()->GetVertexData<VertexData::Pos>();
			const auto& _indeciesData = mesh->GetIndexBuffer()->GetIndecies();

			const uint32_t _triNum = mesh->GetIndexNum() / 3.0f;
			const uint32_t _indexOffset = mesh->GetIndexOffset();
			const uint32_t _vertexOffset = mesh->GetVertexOffset();

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
				if (TriangleTests::Intersects(rayOrigin, rayDirection, v0, v1, v2, d))
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
