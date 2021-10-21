#pragma once

#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Viewport.h"

#include "Core/Event/EventRenderer.h"

namespace TruthEngine
{
	class RenderPass_DeferredShading final : public RenderPass
	{
	public:

		RenderPass_DeferredShading(class RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		virtual void OnImGuiRender() override;
		virtual void OnUpdate(double _DeltaTime) override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void Render() override;

	private:
		void InitRendererCommand() override;
		void InitTextures() override;
		void InitBuffers() override;
		void InitPipelines() override;

		void ReleaseRendererCommand() override;
		void ReleaseTextures() override;
		void ReleaseBuffers() override;
		void ReleasePipelines() override;


		void RegisterEventListeners() override;
		void UnRegisterEventListeners() override;

	private:

		RendererCommand m_RendererCommand;

		PipelineGraphics m_Pipeline;
		
	};
}
