#pragma once

#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"

namespace TruthEngine
{
		class Material;

		class Mesh 
		{
		public:
			Mesh();
			Mesh(size_t ID, size_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum, VertexBufferBase* VertexBufferPtr, IndexBuffer* IndexBufferPtr);

			VertexBufferBase* GetVertexBuffer() const noexcept
			{
				return m_VertexBuffer;
			}

			inline IndexBuffer* GetIndexBuffer() const noexcept
			{
				return m_IndexBuffer;
			}

			inline size_t GetID() const noexcept
			{
				return m_ID;
			}

			inline size_t GetVertexOffset() const noexcept
			{
				return m_VertexOffset;
			}

			inline size_t GetIndexOffset() const noexcept
			{
				return m_IndexOffset;
			}

			inline size_t GetIndexNum() const noexcept
			{
				return m_IndexNum;
			}

			inline size_t GetVertexNum()const noexcept
			{
				return m_VertexNum;
			}

			/*inline AABoundingBox& GetBoundingBox() noexcept
			{
				return m_BoundingBox;
			}

			inline const AABoundingBox& GetBoundingBox()const noexcept
			{
				return m_BoundingBox;
			}*/

		protected:


		protected:
			size_t m_ID = -1;

			size_t m_VertexOffset = 0;
			size_t m_VertexNum = 0;
			size_t m_IndexOffset = 0;
			size_t m_IndexNum = 0;


			VertexBufferBase* m_VertexBuffer = nullptr;
			IndexBuffer* m_IndexBuffer = nullptr;

			//AABoundingBox m_BoundingBox;

			//Friend Classes
			friend class ModelManager;
		};
}
