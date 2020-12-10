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
