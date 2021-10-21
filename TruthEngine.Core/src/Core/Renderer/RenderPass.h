#pragma once

namespace TruthEngine
{

	class RendererLayer;

	class RenderPass
	{
	public:
		RenderPass(TE_IDX_RENDERPASS idx, RendererLayer* _RendererLayer);
		virtual ~RenderPass() = default;

		RenderPass(const RenderPass&) = default;
		RenderPass& operator=(const RenderPass&) = default;

		RenderPass(RenderPass&&) noexcept = default;
		RenderPass& operator=(RenderPass&&) noexcept = default;

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnImGuiRender() = 0;
		virtual void OnUpdate(double _DeltaTime) = 0;

		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;
		virtual void Render() = 0;

	protected:

		virtual void InitRendererCommand() = 0;
		virtual void InitTextures() = 0;
		virtual void InitBuffers() = 0;
		virtual void InitPipelines() = 0;

		virtual void ReleaseRendererCommand() = 0;
		virtual void ReleaseTextures() = 0;
		virtual void ReleaseBuffers() = 0;
		virtual void ReleasePipelines() = 0;


		virtual void RegisterEventListeners() = 0;
		virtual void UnRegisterEventListeners() = 0;

	protected:
		TE_IDX_RENDERPASS m_RenderPassIDX = TE_IDX_RENDERPASS::NONE;
		RendererLayer* m_RendererLayer = nullptr;
	};
}
