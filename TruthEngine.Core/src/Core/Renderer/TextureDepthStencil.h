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

	class TextureDepthStencil : public Texture
	{


	public:
		struct ClearValue
		{
			float depthValue;
			uint8_t stencilValue;
		};

		TextureDepthStencil(
			const char* name
			, uint32_t width
			, uint32_t height
			, TE_RESOURCE_FORMAT format
			, const ClearValue clearValue
			, bool useAsShaderResource
		);

		virtual ~TextureDepthStencil() = default;

		inline const ClearValue GetClearValues() const noexcept
		{
			return m_ClearValue;
		}

		inline void SetClearValues(ClearValue clearValues) noexcept
		{
			m_ClearValue = clearValues;
		}

	private:


	private:
		ClearValue m_ClearValue;

		//
		//friend classes
		//
		friend class TruthEngine::API::DirectX12::DirectX12BufferManager;

	};

}
