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

	/*used D3D12_CLEAR_FLAGS as referece*/
	enum class TE_CLEAR_DEPTH_STENCIL_FLAGS : uint8_t
	{
		CLEAR_DEPTH = 0x1,
		CLEAR_STENCIL = 0x2,
		CLEAR_BOTH = 0x3
	};

	struct ClearValue_DepthStencil
	{

		float depthValue;
		uint8_t stencilValue;
		TE_CLEAR_DEPTH_STENCIL_FLAGS flags = TE_CLEAR_DEPTH_STENCIL_FLAGS::CLEAR_DEPTH;

	};

	class TextureDepthStencil : public Texture
	{


	public:
		

		TextureDepthStencil(
			TE_IDX_GRESOURCES _IDX
			, uint32_t width
			, uint32_t height
			, uint8_t arraySize
			, uint8_t mipLevels
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
