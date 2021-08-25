#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ConstantBuffer.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Viewport.h"

#include "Core/Event/EventRenderer.h"


namespace TruthEngine
{
	class RenderPass_GenerateSSAO : public RenderPass
	{
	public:
		RenderPass_GenerateSSAO(RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		virtual void OnAttach() override;

		virtual void OnDetach() override;

		virtual void OnImGuiRender() override;

		virtual void OnUpdate(double _DeltaTime) override;

		virtual void BeginScene() override;

		virtual void EndScene() override;

		virtual void Render() override;

	private:

		void InitTextures();
		void InitBuffers();
		void InitPipeline();
		void ReleaseTextures();
		void ReleaseBuffers();
		void ReleasePipeline();

		void RegisterEvents();

		void OnEventRendererViewportResize(const EventRendererViewportResize& _Event);

	private:

		RendererCommand m_RendererCommand;
		RendererCommand m_RendererCommand_Blurring;

		TextureRenderTarget* m_TextureRenderTargetSSAO;
		TextureRenderTarget* m_TextureRenderTargetSSAOBlurred;

		Texture* m_TextureNoises;

		RenderTargetView m_RTV;
		RenderTargetView m_RTVBlurred;

		PipelineGraphics m_Pipeline;
		PipelineGraphics m_PipelineBlurring;

		float3A m_KernelSamples[64];

		struct alignas(16) ConstantBufferData_SSAO
		{
			float SSAORadius = 0.5f;
			float SSAODepthBias = 0.025f;
			float2 _Pad0;

			float3A KernelSamples[64];
		};

		ConstantBufferUpload<ConstantBufferData_SSAO>* m_ConstantBufferSSAO;

		float3A m_Noises[16];

	};
}
