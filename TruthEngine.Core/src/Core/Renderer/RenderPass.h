#pragma once

namespace TruthEngine
{
	class RenderPass
	{
	public:
		RenderPass(TE_IDX_RENDERPASS idx);

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnImGuiRender() = 0;

		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;
		virtual void Render() = 0;

	protected:

	protected:
		TE_IDX_RENDERPASS m_RenderPassIDX = TE_IDX_RENDERPASS::NONE;
	};
}
