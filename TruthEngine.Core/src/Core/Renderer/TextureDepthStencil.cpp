#include "pch.h"
#include "TextureDepthStencil.h"

namespace TruthEngine::Core
{

	TextureDepthStencil::TextureDepthStencil(
		const char* name
		, uint32_t width
		, uint32_t height
		, TE_RESOURCE_FORMAT format
		, const ClearValue clearValue
		, bool useAsShaderResource)
		: Texture(name, width, height, format, TE_RESOURCE_USAGE_DEPTHSTENCIL, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::DEPTH_WRITE)
		,m_ClearValue(clearValue)
	{
		if (useAsShaderResource)
		{
			m_Usage = m_Usage | TE_RESOURCE_USAGE_SHADERRESOURCE;
		}
	}

}