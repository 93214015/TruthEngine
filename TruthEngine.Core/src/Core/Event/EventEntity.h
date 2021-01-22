#pragma once
#include "Event.h"

namespace TruthEngine
{

	namespace Core
	{
		class Mesh;
		class Material;
		class ILight;

		class EventEntityAddMesh : public Event
		{
		public:
			EventEntityAddMesh(Mesh* mesh);
			virtual ~EventEntityAddMesh() = default;

			EVENT_CLASS_TYPE(EntityAddMesh);
			EVENT_CLASS_CATEGORY(EventCategoryEntity);

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

			inline Material* GetMaterial() const noexcept
			{
				return m_Material;
			}
		protected:

		protected:
			Material* m_Material;
		};

		class EventEntityUpdateMaterial : public Event
		{
		public:
			EventEntityUpdateMaterial(Material* material);
			~EventEntityUpdateMaterial();

			EVENT_CLASS_TYPE(EntityUpdatedMaterial);
			EVENT_CLASS_CATEGORY(EventCategoryEntity);

			inline Material* GetMaterial() noexcept
			{
				return m_Material;
			}

			inline Material* GetMaterial() const noexcept
			{
				return m_Material;
			}
		protected:

		protected:
			Material* m_Material;
		};

		class EventEntityAddLight : public Event
		{
		public:
			EventEntityAddLight(ILight* light)
				: m_ILight(light)
			{}

			EVENT_CLASS_TYPE(EntityAddLight);
			EVENT_CLASS_CATEGORY(EventCategoryEntity);

			inline ILight* GetLight()const
			{
				return m_ILight;
			}

		private:

		private:
			ILight* m_ILight;
		};

		class EventEntityUpdateLight : public Event
		{
		public:
			EventEntityUpdateLight(ILight* light)
				: m_ILight(light)
			{}

			EVENT_CLASS_TYPE(EntityUpdatedLight);
			EVENT_CLASS_CATEGORY(EventCategoryEntity);

			inline ILight* GetLight()const
			{
				return m_ILight;
			}

		private:

		private:
			ILight* m_ILight;
		};
	}

}