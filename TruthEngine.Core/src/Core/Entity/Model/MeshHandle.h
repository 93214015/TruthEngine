#pragma once

namespace TruthEngine
{
	class MeshHandle
	{
	public:
		MeshHandle() = default;
		MeshHandle(uint32_t _MeshIndex)
			: m_MeshIndex(_MeshIndex)
		{}


		class Mesh& GetMesh();
		const class Mesh& GetMesh() const;

	private:
		uint32_t m_MeshIndex = -1;

		//friend class
		friend class ModelManager;
	};
}
