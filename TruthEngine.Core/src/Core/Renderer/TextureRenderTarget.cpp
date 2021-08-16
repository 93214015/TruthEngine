#include "pch.h"
#include "TextureRenderTarget.h"

namespace TruthEngine
{

	TextureRenderTarget::TextureRenderTarget(
		TE_IDX_GRESOURCES _IDX
		, uint32_t width
		, uint32_t height
		, uint8_t arraySize
		, TE_RESOURCE_TYPE type
		, TE_RESOURCE_FORMAT format
		, ClearValue_RenderTarget clearValue, bool useAsShaderResource, bool enableMSAA)
		: Texture(_IDX, width, height, arraySize, format, TE_RESOURCE_USAGE_RENDERTARGET, type, TE_RESOURCE_STATES::RENDER_TARGET, enableMSAA)
		,m_ClearValue(clearValue)
	{
		if (useAsShaderResource)
		{
			m_Usage = m_Usage | TE_RESOURCE_USAGE_SHADERRESOURCE;
		}
	}

}