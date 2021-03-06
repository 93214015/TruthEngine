#pragma once

#include "Core/Renderer/Material.h"

namespace TruthEngine
{
		class MaterialComponent
		{
		public:
			MaterialComponent(Material* material)
				: m_Material(material)
			{}


			inline Material* GetMaterial() noexcept
			{
				return m_Material;
			}

			inline Material* GetMaterial() const noexcept
			{
				return m_Material;
			}

		private:
			Material* m_Material;

			//
			//Friend Classes
			//
			friend class MaterialManager;
		};
}
