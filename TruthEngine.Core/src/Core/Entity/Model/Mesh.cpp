#include "pch.h"
#include "Mesh.h"

namespace TruthEngine::Core
{
	Mesh::Mesh() = default;


	Mesh::Mesh(uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset)
		: m_IndexNum(IndexNum), m_IndexOffset(static_cast<uint32_t>(IndexOffset)), m_VertexOffset(static_cast<uint32_t>(VertexOffset))
	{

	}


}