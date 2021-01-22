#pragma once

namespace TruthEngine::Core
{

	class BoundingBoxComponent
	{
	public:
		BoundingBoxComponent();
		BoundingBoxComponent(const BoundingBox& boundingBox);
		BoundingBoxComponent(size_t vertexNum, const float3* vertecies, size_t strideSize);

		void Init(size_t vertexNum, const float3* vertecies, size_t strideSize);

		const BoundingBox& GetBoundingBox()const noexcept
		{
			return m_BoundingBox;
		}

	private:

	private:
		BoundingBox m_BoundingBox;
	};
}
