#pragma once
#include "Texture.h"

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12
		{
			class DirectX12BufferManager;
		}
	}
}

namespace TruthEngine
{


	struct ClearValue_RenderTarget
	{
		float x;
		float y;
		float z;
		float w;
	};


	class TextureRenderTarget : public Texture
	{
	public:

		TextureRenderTarget(
			TE_IDX_GRESOURCES _IDX
			, uint32_t width
			, uint32_t height
			, uint8_t arraySize
			, uint8_t mipLevels
			, TE_RESOURCE_TYPE type
			, TE_RESOURCE_FORMAT format
			, ClearValue_RenderTarget clearValue
			, bool useAsShaderResource
			, bool enableMSAA
		);

		virtual ~TextureRenderTarget() = default;


		inline void SetClearValues(const ClearValue_RenderTarget& clearValue)
		{
			m_ClearValue = clearValue;
		}

		inline ClearValue_RenderTarget GetClearValues() const noexcept
		{
			return m_ClearValue;
		}

	private:


	private:
		ClearValue_RenderTarget m_ClearValue;


		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DirectX12BufferManager;

	};

}
