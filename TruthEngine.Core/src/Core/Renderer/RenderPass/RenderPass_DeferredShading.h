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
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnUpdate(double _DeltaTime) override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void Render() override;

	private:

		void InitTexture();
		void InitBuffer();

		void PreparePipeline();

		void RegisterEvents();

		//Event Listeners
		void OnSceneViewportResize(EventRendererViewportResize& _Event);

	private:

		RendererCommand m_RendererCommand;

		RenderTargetView m_RTVSceneBuffer;

		PipelineGraphics m_Pipeline;

		Viewport m_Viewport;
		ViewRect m_ViewRect;
		
	};
}
