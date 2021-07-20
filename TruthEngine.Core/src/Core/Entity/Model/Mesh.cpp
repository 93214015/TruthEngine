#include "pch.h"
#include "Mesh.h"

namespace TruthEngine
{
	Mesh::Mesh() = default;


	Mesh::Mesh(size_t ID, size_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum, VertexBufferBase* VertexBufferPtr, IndexBuffer* IndexBufferPtr)
		: m_ID(ID), m_VertexOffset(VertexOffset), m_VertexNum(vertexNum), m_IndexOffset(IndexOffset), m_IndexNum(IndexNum), m_VertexBuffer(VertexBufferPtr), m_IndexBuffer(IndexBufferPtr)
	{
		assert(m_IndexBuffer != nullptr && m_VertexBuffer != nullptr);
	}


}