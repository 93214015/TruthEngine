#include "pch.h"
#include "TextureDepthStencil.h"

namespace TruthEngine
{

	TextureDepthStencil::TextureDepthStencil(
		TE_IDX_GRESOURCES _IDX
		, uint32_t width
		, uint32_t height
		, TE_RESOURCE_FORMAT format
		, const ClearValue_DepthStencil clearValue
		, bool useAsShaderResource
		, bool enbaleMSAA)
		: Texture(_IDX, width, height, format, TE_RESOURCE_USAGE_DEPTHSTENCIL, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::DEPTH_WRITE, enbaleMSAA)
		, m_ClearValue(clearValue)
	{
		if (useAsShaderResource)
		{
			m_Usage = m_Usage | TE_RESOURCE_USAGE_SHADERRESOURCE;
		}
	}

}