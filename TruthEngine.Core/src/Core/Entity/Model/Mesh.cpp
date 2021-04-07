#include "pch.h"
#include "Mesh.h"

namespace TruthEngine
{
	Mesh::Mesh() = default;


	Mesh::Mesh(uint32_t ID, uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum, const BoundingBox& boundingBox, VertexBufferBase* VertexBufferPtr, IndexBuffer* IndexBufferPtr)
		: m_ID(ID), m_IndexNum(IndexNum), m_IndexOffset(static_cast<uint32_t>(IndexOffset)), m_VertexOffset(static_cast<uint32_t>(VertexOffset)), m_VertexNum(static_cast<uint32_t>(vertexNum)), m_VertexBuffer(VertexBufferPtr), m_IndexBuffer(IndexBufferPtr), m_BoundingBox(boundingBox)
	{}


}