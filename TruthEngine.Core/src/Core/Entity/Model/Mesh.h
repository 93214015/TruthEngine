#pragma once

#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"

namespace TruthEngine
{

	namespace Core
	{
		class Material;

		class Mesh 
		{
		public:
			Mesh();
			Mesh(uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum, const BoundingBox& boundingBox, VertexBuffer<VertexData::Pos, VertexData::NormTanTex>* VertexBufferPtr, IndexBuffer* IndexBufferPtr);

			VertexBuffer<VertexData::Pos, VertexData::NormTanTex>* GetVertexBuffer() const noexcept
			{
				return m_VertexBuffer;
			}

			inline IndexBuffer* GetIndexBuffer() const noexcept
			{
				return m_IndexBuffer;
			}

			inline uint32_t GetVertexOffset() const noexcept
			{
				return m_VertexOffset;
			}

			inline uint32_t GetIndexOffset() const noexcept
			{
				return m_IndexOffset;
			}

			inline int32_t GetIndexNum() const noexcept
			{
				return m_IndexNum;
			}

			inline uint32_t GetVertexNum()const noexcept
			{
				return m_VertexNum;
			}

			inline const BoundingBox& GetBoundingBox()const noexcept
			{
				return m_BoundingBox;
			}

		protected:


		protected:
			VertexBuffer<VertexData::Pos, VertexData::NormTanTex>* m_VertexBuffer = nullptr;
			IndexBuffer* m_IndexBuffer = nullptr;

			uint32_t m_VertexOffset = 0;
			uint32_t m_VertexNum = 0;
			uint32_t m_IndexOffset = 0;
			uint32_t m_IndexNum = 0;

			BoundingBox m_BoundingBox;

			//Friend Classes
			friend class ModelManager;
		};
	}
}
