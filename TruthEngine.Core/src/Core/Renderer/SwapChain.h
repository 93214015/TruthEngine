#pragma once

#include "Core/Application.h"

#include "TextureRenderTarget.h"


namespace TruthEngine {

	struct SwapChainRenderTargets
	{
		uint32_t RenderTargetIndex;
	};

	class SwapChain {

	public:
		virtual void Present() = 0;

		virtual TE_RESULT Init(uint32_t clientWidth, uint32_t clientHeight, Window* outputWindow, uint32_t backBufferNum = 2) = 0;

		virtual void Release() = 0;

		virtual TE_RESULT Resize(uint32_t width, uint32_t height, uint32_t backBufferNum) = 0;

		virtual uint8_t GetCurrentFrameIndex() const = 0;

		inline const ClearValue_RenderTarget GetClearValues() const noexcept
		{
			return m_ClearValues;
		}

		inline void SetClearValues(const ClearValue_RenderTarget clearValues) noexcept
		{
			m_ClearValues = clearValues;
		}

		inline uint8_t GetBackBufferNum()const noexcept { return m_BackBufferNum; }


		static SwapChain* GetInstance();

	protected:

		ClearValue_RenderTarget m_ClearValues = { 1.0f, 1.0f, 1.0f, 1.0f};

		uint8_t m_BackBufferNum = 2;

		bool m_UseMSAA4X = false;

	};

}

#define TE_INSTANCE_SWAPCHAIN TruthEngine::SwapChain::GetInstance()
