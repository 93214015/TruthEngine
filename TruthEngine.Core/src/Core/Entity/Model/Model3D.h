#pragma once
#include "Model.h"

namespace TruthEngine
{
	namespace Core 
	{

		class Material;
		class Mesh;

		class Model3D : public Model
		{
		public:
			inline const std::vector<Mesh*>& GetMeshes() const noexcept
			{
				return m_Meshes;
			}

			inline void AddMesh(Mesh* mesh) noexcept
			{
				assert(mesh);
				m_Meshes.push_back(mesh);
			}

			inline void AddSpace(size_t size)
			{
				auto currentSpace = m_Meshes.size();
				m_Meshes.reserve(currentSpace + size);
			}

		protected:
			

		protected:
			std::vector<Mesh*> m_Meshes;

			//
			////Friend Classes
			//
			friend class ModelManager;

		};
	}
}
