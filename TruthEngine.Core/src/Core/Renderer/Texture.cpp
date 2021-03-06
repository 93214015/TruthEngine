#include "pch.h"
#include "Texture.h"

namespace TruthEngine
{
	
	Texture::Texture(TE_IDX_GRESOURCES _IDX, uint32_t width, uint32_t height, uint8_t arraySize, uint8_t mipLevels, TE_RESOURCE_FORMAT format, TE_RESOURCE_USAGE usage, TE_RESOURCE_TYPE type, TE_RESOURCE_STATES initState, bool enableMSAA)
		: GraphicResource(_IDX, usage, type, initState), m_Width(width), m_Height(height), m_ArraySize(arraySize), m_MipLevels(mipLevels), m_Format(format), m_EnableMSAA(enableMSAA)
	{
	}

}