#pragma once

#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{
	class RenderPass_GenerateIBL final : public RenderPass
	{
	public:
		RenderPass_GenerateIBL(RendererLayer* _RendererLayer);

		void OnAttach() override;
		void OnDetach() override;

		void OnImGuiRender() override;
		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;
		void EndScene() override;

		void Render() override;

		void Initialize(TE_IDX_GRESOURCES _TextureIBLIDX, size_t _TextureIBLSize, TE_RESOURCE_FORMAT _TextureIBLFormat, const char* _OutputFilePath);

	private:
		void PreparePipline();

		void InitTextures();
		void InitBuffers();
	private:

		bool m_IsInitialized = false;
		TE_IDX_GRESOURCES m_TextureIBLIDX = TE_IDX_GRESOURCES::NONE;
		TE_RESOURCE_FORMAT m_TextureIBLFormat = TE_RESOURCE_FORMAT::UNKNOWN;
		size_t m_TextureIBLSize = 1024;

		std::string m_OutputTextureFilePath = "";

		RendererCommand m_RendererCommand;

		RenderTargetView m_RenderTartgetViewIBL;

		TextureRenderTarget* m_TextureRenderTargetIBL = nullptr;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		PipelineGraphics m_PipelineGenerateIBL;
	};
}
