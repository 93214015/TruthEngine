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
			MeshComponent(const Mesh& _Mesh)
				: m_Mesh(_Mesh)
			{}

			const Mesh& GetMesh() const
			{
				return m_Mesh;
			}

		private:


		private:
			Mesh m_Mesh;

			//Friend Classes
			friend class ModelManager;
		};
}
