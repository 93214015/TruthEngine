#pragma once
#include "Event.h"

namespace TruthEngine
{

	namespace Core
	{
		class Mesh;
		class Material;

		class EventEntityAddMesh : public Event
		{
		public:
			EventEntityAddMesh(Mesh* mesh);
			virtual ~EventEntityAddMesh() = default;

			EVENT_CLASS_TYPE(EntityAddMesh)
			EVENT_CLASS_CATEGORY(EventCategoryEntity)

			inline Mesh* GetMesh() noexcept
			{
				return m_Mesh;
			}
		protected:

		protected:
			Mesh* m_Mesh;
		};

		class EventEntityAddMaterial : public Event
		{
		public:
			EventEntityAddMaterial(Material* material);
			~EventEntityAddMaterial();

			EVENT_CLASS_TYPE(EntityAddMaterial)
			EVENT_CLASS_CATEGORY(EventCategoryEntity)

			inline Material* GetMaterial() noexcept
			{
				return m_Material;
			}
		protected:

		protected:
			Material* m_Material;
		};
	}

}