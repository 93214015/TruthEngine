#pragma once

#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"

#include "Core/Entity/Entity.h"
#include "Core/Entity/Model/Mesh.h"

namespace TruthEngine
{
		class MeshComponent
		{
		public:
			MeshComponent(Mesh* mesh)
				: m_Mesh(mesh)
			{}

			inline Mesh* GetMesh() noexcept
			{
				return m_Mesh;
			}


		private:


		private:
			Mesh* m_Mesh;

			//Friend Classes
			friend class ModelManager;
		};
}
