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
			Mesh(uint32_t ID, uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum, const BoundingBox& boundingBox, VertexBuffer<VertexData::Pos, VertexData::NormTanTex>* VertexBufferPtr, IndexBuffer* IndexBufferPtr);

			VertexBuffer<VertexData::Pos, VertexData::NormTanTex>* GetVertexBuffer() const noexcept
			{
				return m_VertexBuffer;
			}

			inline uint32_t GetID() const noexcept
			{
				return m_ID;
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

			inline BoundingBox& GetBoundingBox() noexcept
			{
				return m_BoundingBox;
			}

			inline const BoundingBox& GetBoundingBox()const noexcept
			{
				return m_BoundingBox;
			}

		protected:


		protected:
			uint32_t m_ID = -1;

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
