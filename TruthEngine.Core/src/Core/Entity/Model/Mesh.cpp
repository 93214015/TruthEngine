#include "pch.h"
#include "Mesh.h"

namespace TruthEngine
{

	Mesh::Mesh(size_t ID, size_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum, VertexBufferBase* VertexBufferPtr, IndexBuffer* IndexBufferPtr)
		: m_ID(ID), m_VertexOffset(static_cast<uint32_t>(VertexOffset)), m_VertexNum(static_cast<uint32_t>(vertexNum)), m_IndexOffset(static_cast<uint32_t>(IndexOffset)), m_IndexNum(static_cast<uint32_t>(IndexNum)), m_VertexBuffer(VertexBufferPtr), m_IndexBuffer(IndexBufferPtr)
	{
		assert(m_IndexBuffer != nullptr && m_VertexBuffer != nullptr);
	}


}