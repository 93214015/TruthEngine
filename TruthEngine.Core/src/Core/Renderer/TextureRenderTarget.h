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

namespace TruthEngine::Core
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
			uint32_t width
			, uint32_t height
			, TE_RESOURCE_FORMAT format
			, ClearValue_RenderTarget clearValue
			, bool useAsShaderResource
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
