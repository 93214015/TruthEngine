#pragma once

#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{

	class RenderPass_GenerateCubeMap final : public RenderPass
	{
	public:
		RenderPass_GenerateCubeMap(RendererLayer* _RendererLayer);

		void OnAttach() override;
		void OnDetach() override;

		void OnImGuiRender() override;
		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;
		void EndScene() override;

		void Render() override;


	private:
		void PreparePipline();

		void InitTextures();
		void InitBuffers();
	private:

		RendererCommand m_RendererCommand;
		RendererCommand m_RendererCommand_ResolveTextures;

		RenderTargetView m_RenderTartgetView;
		DepthStencilView m_DepthStencilView;

		TextureDepthStencil* m_TextureDepthStencil = nullptr;
		TextureDepthStencil* m_TextureDepthStencilMS = nullptr;
		TextureRenderTarget* m_TextureRenderTargetHDRMS = nullptr;
		TextureRenderTarget* m_TextureRenderTargetMS = nullptr;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		class MaterialManager* m_MaterialManager;

		PipelineGraphics m_Pipeline;
	};
}