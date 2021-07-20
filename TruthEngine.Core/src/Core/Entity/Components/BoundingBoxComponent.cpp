#include "pch.h"
#include "BoundingBoxComponent.h"

#include "Core/Entity/Model/Mesh.h"

namespace TruthEngine
{
	BoundingBoxComponent::BoundingBoxComponent()
	{}
	BoundingBoxComponent::BoundingBoxComponent(const BoundingAABox & boundingBox)
		: m_AABoundingBox(boundingBox)
	{}
	BoundingBoxComponent::BoundingBoxComponent(size_t vertexNum, const float3 * vertecies, size_t strideSize)
	{
		Init(vertexNum, vertecies, strideSize);
	}
	BoundingBoxComponent::BoundingBoxComponent(const Mesh& _Mesh)
	{
		CreateBoundingAABoxFromPoints(m_AABoundingBox, _Mesh.GetVertexNum(), &_Mesh.GetVertexBuffer()->GetPosData()[_Mesh.GetVertexOffset()].Position, sizeof(VertexData::Pos));
	}
	void BoundingBoxComponent::Init(size_t vertexNum, const float3* vertecies, size_t strideSize)
	{
		CreateBoundingAABoxFromPoints(m_AABoundingBox, vertexNum, vertecies, strideSize);
	}

}