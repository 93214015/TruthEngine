#pragma once

namespace TruthEngine
{
	class MeshHandle
	{
	public:
		MeshHandle(uint32_t _MeshIndex)
			: m_MeshIndex(_MeshIndex)
		{}


		class Mesh& GetMesh();

	private:
		uint32_t m_MeshIndex;

		//friend class
		friend class ModelManager;
	};
}
