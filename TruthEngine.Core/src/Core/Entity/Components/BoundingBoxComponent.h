#pragma once

namespace TruthEngine
{

	class BoundingBoxComponent
	{
	public:
		BoundingBoxComponent();
		BoundingBoxComponent(const BoundingAABox& boundingBox);
		BoundingBoxComponent(size_t vertexNum, const float3* vertecies, size_t strideSize);
		BoundingBoxComponent(const class Mesh& _Mesh);


		void Init(size_t vertexNum, const float3* vertecies, size_t strideSize);

		const BoundingAABox& GetBoundingBox()const noexcept
		{
			return m_AABoundingBox;
		}

		BoundingAABox& GetBoundingBox() noexcept
		{
			return m_AABoundingBox;
		}

	private:

	private:
		BoundingAABox m_AABoundingBox;
	};
}
