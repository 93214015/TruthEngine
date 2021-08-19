#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{
	class RenderPass_GenerateIBLSpecular : public RenderPass
	{
	public:
		RenderPass_GenerateIBLSpecular(RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnUpdate(double _DeltaTime) override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void Render() override;

		void Initialize(size_t _TextureIBLSize, size_t _MipMapLevels, TE_RESOURCE_FORMAT _TextureIBLFormat);

	private:
		void InitTextures();
		void InitBuffers();
		void InitPipeline();

	private:
		bool m_IsInitialized = false;
		uint32_t m_TextureIBLSize;
		uint32_t m_MipMapLevels;
		TE_RESOURCE_FORMAT m_TextureIBLFormat;

		RendererCommand m_RendererCommand;

		PipelineGraphics m_Pipeline;

		TextureRenderTarget* m_RenderTargetIBLSpecular;

		std::vector<RenderTargetView> m_RTVs;

		Viewport m_Viewport;
		ViewRect m_ViewRect;
	};
}
