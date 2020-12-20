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

	

	class TextureRenderTarget : public Texture
	{
	public:

		struct ClearValue
		{
			float x;
			float y;
			float z;
			float w;
		};

		TextureRenderTarget(
			  const char* name
			, uint32_t width
			, uint32_t height
			, TE_RESOURCE_FORMAT format
			, ClearValue clearValue
			, bool useAsShaderResource
		);

		virtual ~TextureRenderTarget() = default;

		inline uint32_t GetWidth() const noexcept { return m_Width; }
		inline uint32_t GetHeight() const noexcept { return m_Height; }
		inline TE_RESOURCE_FORMAT GetFormat() const noexcept { return m_Format; }

		inline void SetClearValues(const ClearValue& clearValue)
		{
			m_ClearValue = clearValue;
		}

		inline ClearValue GetClearValues() const noexcept
		{
			return m_ClearValue;
		}

	private:


	private:
		ClearValue m_ClearValue;


		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DirectX12BufferManager;

	};

}
