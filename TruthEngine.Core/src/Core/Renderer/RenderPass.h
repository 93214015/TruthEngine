#pragma once

namespace TruthEngine
{


	namespace Core
	{
		class RenderPass
		{
		public:
			RenderPass(TE_IDX_RENDERPASS idx);
		protected:

		protected:
			TE_IDX_RENDERPASS m_RenderPassIDX = TE_IDX_RENDERPASS::NONE;
		};
	}
}
