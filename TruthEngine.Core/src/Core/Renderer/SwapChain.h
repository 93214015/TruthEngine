#pragma once

#include "Core/Application.h"

#include "TextureRenderTarget.h"


namespace TruthEngine::Core {

	struct SwapChainRenderTargets
	{
		uint32_t RenderTargetIndex;
	};

	class SwapChain {

	public:
		virtual void Present() = 0;

		virtual TE_RESULT Init(UINT clientWidth, UINT clientHeight, Window* outputWindow, UINT backBufferNum = 2) = 0;

		virtual void Release() = 0;

		virtual TE_RESULT Resize(UINT width, UINT height, UINT backBufferNum) = 0;

		inline const ClearValue_RenderTarget GetClearValues() const noexcept
		{
			return m_ClearValues;
		}

		inline void SetClearValues(const ClearValue_RenderTarget clearValues) noexcept
		{
			m_ClearValues = clearValues;
		}

		inline UINT GetBackBufferNum()const noexcept { return m_BackBufferNum; }

		inline UINT GetCurrentFrameIndex() const noexcept 
		{
			auto app = TE_INSTANCE_APPLICATION;
			return app->GetCurrentFrameIndex(); 
		};

		static SwapChain* GetInstance();

	protected:

		ClearValue_RenderTarget m_ClearValues = { 1.0f, 1.0f, 1.0f, 1.0f};

		UINT m_BackBufferNum = 2;

		bool m_UseMSAA4X = false;

	};

}

#define TE_INSTANCE_SWAPCHAIN TruthEngine::Core::SwapChain::GetInstance()