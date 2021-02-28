#include "pch.h"
#include "Texture.h"

namespace TruthEngine
{
	
	Texture::Texture(uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, TE_RESOURCE_USAGE usage, TE_RESOURCE_TYPE type, TE_RESOURCE_STATES initState, bool enableMSAA)
		: GraphicResource(usage, type, initState), m_Width(width), m_Height(height), m_Format(format), m_EnableMSAA(enableMSAA)
	{
	}

}