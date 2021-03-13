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

	struct ClearValue_DepthStencil
	{
		float depthValue;
		uint8_t stencilValue;
	};

	class TextureDepthStencil : public Texture
	{


	public:
		

		TextureDepthStencil(
			TE_IDX_GRESOURCES _IDX
			, uint32_t width
			, uint32_t height
			, TE_RESOURCE_FORMAT format
			, const ClearValue_DepthStencil clearValue
			, bool useAsShaderResource
			, bool enableMSAA
		);

		virtual ~TextureDepthStencil() = default;

		inline const ClearValue_DepthStencil GetClearValues() const noexcept
		{
			return m_ClearValue;
		}

		inline void SetClearValues(ClearValue_DepthStencil clearValues) noexcept
		{
			m_ClearValue = clearValues;
		}

	private:


	private:
		ClearValue_DepthStencil m_ClearValue;

		//
		//friend classes
		//
		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DirectX12BufferManager;

	};

}
