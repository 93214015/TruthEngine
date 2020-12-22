#pragma once

namespace TruthEngine
{

	namespace Core
	{
		class RenderPass
		{
		public:
			RenderPass(TE_IDX_RENDERPASS idx);

			virtual void OnAttach() = 0;
			virtual void OnDetach() = 0;
			virtual void OnImGuiRender() = 0;

			virtual void OnResize(uint32_t width, uint32_t height) = 0;
		protected:

		protected:
			TE_IDX_RENDERPASS m_RenderPassIDX = TE_IDX_RENDERPASS::NONE;
		};
	}
}
