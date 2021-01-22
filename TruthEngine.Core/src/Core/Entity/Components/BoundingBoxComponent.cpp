#include "pch.h"
#include "BoundingBoxComponent.h"

namespace TruthEngine::Core
{
	BoundingBoxComponent::BoundingBoxComponent()
	{}
	BoundingBoxComponent::BoundingBoxComponent(const BoundingBox & boundingBox)
		: m_BoundingBox(boundingBox)
	{}
	BoundingBoxComponent::BoundingBoxComponent(size_t vertexNum, const float3 * vertecies, size_t strideSize)
	{
		Init(vertexNum, vertecies, strideSize);
	}
	void BoundingBoxComponent::Init(size_t vertexNum, const float3* vertecies, size_t strideSize)
	{
		CreateBoundingBoxFromPoints(m_BoundingBox, vertexNum, vertecies, strideSize);
	}
}