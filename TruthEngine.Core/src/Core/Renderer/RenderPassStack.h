#pragma once

namespace TruthEngine::Core
{
	class RenderPass;

	class RenderPassStack
	{
	public:

		void PushRenderPass(RenderPass* renderPass);
		void PopRenderPass(RenderPass* renderPass);

	private:
		std::vector<RenderPass*> m_Vector;

	};

}
