#include "pch.h"
#include "TextureRenderTarget.h"

namespace TruthEngine::Core
{

	TextureRenderTarget::TextureRenderTarget(uint32_t width
		, uint32_t height, TE_RESOURCE_FORMAT format
		, ClearValue_RenderTarget clearValue, bool useAsShaderResource)
		: Texture(width, height, format, TE_RESOURCE_USAGE_RENDERTARGET, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::RENDER_TARGET)
		,m_ClearValue(clearValue)
	{
		if (useAsShaderResource)
		{
			m_Usage = m_Usage | TE_RESOURCE_USAGE_SHADERRESOURCE;
		}
	}

}