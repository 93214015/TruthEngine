#pragma once



namespace TruthEngine::Core {

	class SwapChain {

	public:
		virtual void Present() = 0;

		static SwapChain* Get();
	protected:

	};

}

#define TE_INSTANCE_SWAPCHAIN TruthEngine::Core::SwapChain::Get()