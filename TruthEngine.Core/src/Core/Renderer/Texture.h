#pragma once
#include "GraphicResource.h"

namespace TruthEngine
{


	namespace Core
	{
		class Texture : public GraphicResource
		{
		public:

			Texture(const char* name
				, uint32_t width
				, uint32_t height
				, TE_RESOURCE_FORMAT format
				, TE_RESOURCE_USAGE usage
				, TE_RESOURCE_TYPE type
				, TE_RESOURCE_STATES initState);


		protected:


		protected:
			uint32_t m_Width = 0, m_Height = 0;
			TE_RESOURCE_FORMAT m_Format = TE_RESOURCE_FORMAT::UNKNOWN;
		};
	}
}
