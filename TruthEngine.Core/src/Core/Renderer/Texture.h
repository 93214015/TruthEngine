#pragma once
#include "GraphicResource.h"

namespace TruthEngine
{


	namespace Core
	{
		class Texture : public GraphicResource
		{
		public:

			Texture( uint32_t width
				, uint32_t height
				, TE_RESOURCE_FORMAT format
				, TE_RESOURCE_USAGE usage
				, TE_RESOURCE_TYPE type
				, TE_RESOURCE_STATES initState);

			virtual ~Texture() = default;

			inline void Resize(uint32_t width, uint32_t height) noexcept
			{
				m_Width = width; m_Height = height;
			}

		protected:


		protected:
			uint32_t m_Width = 0, m_Height = 0, m_MipLevels = 1;
			TE_RESOURCE_FORMAT m_Format = TE_RESOURCE_FORMAT::UNKNOWN;
			
		};
	}
}
