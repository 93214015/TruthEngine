#include "pch.h"
#include "PickingEntity.h"

#include "Core/Entity/Entity.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Components.h"
#include "Core/Entity/Model/Mesh.h"


#include "Core/Application.h"


void TruthEngine::PickingEntity::PickEntity(float3 rayOrigin, float3 rayDirection, Scene* scene, Camera* camera)
{
}

void TruthEngine::PickingEntity::PickEntity(float2 mousePosition, Scene* scene, Camera* camera)
{
	const auto& _proj = camera->GetProjection();

	float vx = (2.0f * mousePosition.x / TE_INSTANCE_APPLICATION->GetSceneViewportWidth() - 1.0f) / _proj(0, 0);
	float vy = (-2.0f * mousePosition.y / TE_INSTANCE_APPLICATION->GetSceneViewportHeight() + 1.0f) / _proj(1, 1);


	const float3& cameraPosition = camera->GetPosition();
	XMVector rayOrigin = Math::XMVectorSet(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
	XMVector rayDirection = Math::XMVectorSet(vx, vy, 1.0f, 0.0f);

	const float4x4A& _view = camera->GetView();
	XMMatrix _viewInv = Math::XMInverse(Math::ToXM(_view));

	rayDirection = Math::XMTransformVector3Normal(rayDirection, _viewInv);

	auto& _entityGroup = scene->ViewEntities<MeshComponent, BoundingBoxComponent>();

	float _closestMeshDistance = FLT_MAX;

	for (auto entity : _entityGroup)
	{
		auto tag = scene->GetComponent<TagComponent>(entity).GetTag();

		float4x4A _world = scene->GetTransformHierarchy(entity);
		auto _scaleVector = Math::XMVectorSet(_world._11, _world._22, _world._33, 0.0f);
		_world._11 = 1.0f;
		_world._22 = 1.0f;
		_world._33 = 1.0f;

		XMMatrix _worldInv = Math::XMInverse(Math::ToXM(_world));

		rayOrigin = Math::XMTransformVector3Point(rayOrigin, _worldInv);
		rayDirection = Math::XMTransformVector3Normal(rayDirection, _worldInv);
		rayDirection = Math::XMNormalizeEst(rayDirection);


		float _aabbDistance = 0.0f;
		bool _aabbIntersected = false;
		{
			BoundingAABox _aabbScaled;
			scene->GetComponent<BoundingBoxComponent>(entity).GetBoundingBox().Transform(_aabbScaled, Math::XMTransformMatrixScale(_scaleVector));
			_aabbIntersected = _aabbScaled.Intersects(rayOrigin, rayDirection, _aabbDistance);
		}

		if (_aabbIntersected)
		{
			const Mesh& mesh = scene->GetComponent<MeshComponent>(entity).GetMesh();

			const auto& _verteciesData = mesh.GetVertexBuffer()->GetPosData();
			const auto& _indeciesData = mesh.GetIndexBuffer()->GetIndecies();

			const size_t _TriangleNum = mesh.GetIndexNum() / 3;
			const size_t _indexOffset = mesh.GetIndexOffset();
			const size_t _vertexOffset = mesh.GetVertexOffset();

			for (size_t _TriangleIndex = 0; _TriangleIndex < _TriangleNum; ++_TriangleIndex)
			{
				size_t _iindex = _indexOffset + (_TriangleIndex * 3);

				size_t i0 = _indeciesData[_iindex];
				size_t i1 = _indeciesData[_iindex + 1];
				size_t i2 = _indeciesData[_iindex + 2];

				XMVector v0 = Math::XMMultiply(Math::ToXM(_verteciesData[i0 + _vertexOffset].Position), _scaleVector);
				XMVector v1 = Math::XMMultiply(Math::ToXM(_verteciesData[i1 + _vertexOffset].Position), _scaleVector);
				XMVector v2 = Math::XMMultiply(Math::ToXM(_verteciesData[i2 + _vertexOffset].Position), _scaleVector);

				float d = 0.0f;

				if (Math::TriangleTests::Intersects(rayOrigin, rayDirection, v0, v1, v2, d))
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
