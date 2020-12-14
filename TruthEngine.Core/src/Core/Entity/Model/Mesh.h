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

			VertexBuffer<VertexData::Pos, VertexData::NormTex>* GetVertexBuffer() const noexcept
			{
				return m_VertexBuffer;
			}

			inline IndexBuffer* GetIndexBuffer() const noexcept
			{
				return m_IndexBuffer;
			}

			inline Material* GetMaterial() const noexcept
			{
				return m_Material;
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

		protected:


		protected:
			Material* m_Material = nullptr;
			VertexBuffer<VertexData::Pos, VertexData::NormTex>* m_VertexBuffer = nullptr;
			IndexBuffer* m_IndexBuffer = nullptr;

			uint32_t m_VertexOffset = 0;
			uint32_t m_IndexOffset = 0;
			uint32_t m_IndexNum = 0;


			//Friend Classes
			friend class ModelManager;
		};
	}
}