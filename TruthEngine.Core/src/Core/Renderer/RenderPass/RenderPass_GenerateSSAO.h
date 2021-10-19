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
	class RenderPass_GenerateSSAO final : public RenderPass
	{
	public:
		RenderPass_GenerateSSAO(RendererLayer* _RendererLayer);

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

		void SetThreadNum(uint32_t Width, uint32_t Height);

		void OnEventRendererViewportResize(const EventRendererViewportResize& _Event);

	private:

		RendererCommand m_RendererCommand;
		RendererCommand m_RendererCommand_Blurring;
		RendererCommand m_RendererCommand_BlurringCS;
		RendererCommand m_RendererCommand_MedianBlurring;
		RendererCommand m_RendererCommand_MedianBlurringCS;

		TextureRenderTarget* m_TextureRenderTargetSSAO;
		TextureRenderTarget* m_TextureRenderTargetSSAOBlurred;
		TextureRenderTarget* m_TextureRenderTargetSSAOBlurredMedian;

		Texture* m_TextureNoises;
		Texture* m_TextureRWBlurredMedian;
		Texture* m_TextureRWBlurred;

		RenderTargetView m_RTV;
		RenderTargetView m_RTVBlurred;
		RenderTargetView m_RTVBlurredMedian;


		PipelineGraphics m_Pipeline;
		PipelineGraphics m_PipelineBlurring;
		PipelineGraphics m_PipelineBlurringMedian;
		PipelineCompute m_PipelineBlurringCS;
		PipelineCompute m_PipelineBlurringMedianCS;


		struct alignas(16) ConstantBufferData_SSAO
		{
			float SSAORadius = 0.5f;
			float SSAODepthBias = 0.025f;
			float2 _Pad0;

			float3A KernelSamples[64];
		};

		struct alignas(16) ConstantBufferData_SSAOBlurringCS
		{
			uint2 TextureSize;
			uint2 _Pad0;
		};

		ConstantBufferUpload<ConstantBufferData_SSAO>* m_ConstantBufferSSAO;
		ConstantBufferUpload<ConstantBufferData_SSAOBlurringCS>* m_ConstantBufferBlurringCS;

		float3 m_Noises[16];
		float3A m_KernelSamples[64];

		uint2 m_MedianBlurringDispatchThreadNum = uint2{ 0, 0 };

		std::vector<EventListenerHandle> m_EventListenerList;

	};
}
