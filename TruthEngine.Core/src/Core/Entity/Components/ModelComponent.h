#pragma once
#include "Core/Entity/Model/Model.h"


namespace TruthEngine
{
		class ModelComponent
		{
		public:
			ModelComponent();
			ModelComponent(const std::vector<Entity>& _MeshEntities);
			

			inline void ReserveMesh(size_t _MeshNum)
			{
				m_EnitiesMesh.reserve(_MeshNum);
			}

			inline std::vector<Entity>& GetMeshEntities()
			{
				return m_EnitiesMesh;
			}

		private:
			inline void AddMeshEntity(Entity _MeshEntity)
			{
				m_EnitiesMesh.emplace_back(_MeshEntity);
			}


		private:
			uint32_t m_ID = -1;

			std::vector<Entity> m_EnitiesMesh;


			//friend class
			friend class Scene;
		};
}