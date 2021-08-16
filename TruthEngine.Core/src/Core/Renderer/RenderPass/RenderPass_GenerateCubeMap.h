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

		RenderTargetView m_RenderTartgetView;

		TextureRenderTarget* m_TextureRenderTargetCubeMap = nullptr;
		Texture m_TextureIBL;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		class MaterialManager* m_MaterialManager;

		PipelineGraphics m_Pipeline;
	};
}