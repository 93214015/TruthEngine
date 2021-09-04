#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{
	class RenderPass_GenerateIBLSpecular final : public RenderPass
	{
	public:
		RenderPass_GenerateIBLSpecular(RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		virtual void OnImGuiRender() override;
		virtual void OnUpdate(double _DeltaTime) override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void Render() override;

		void Initialize(size_t _TextureIBLPrefilterEnvironmentSize, size_t _TextureIBLPrecomputeBRDFSize, size_t _MipMapLevels, TE_RESOURCE_FORMAT _TextureIBLFormat);

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
		bool m_IsInitialized = false;
		uint32_t m_TextureIBLPrefilterEnvironmentSize;
		uint32_t m_TextureIBLPrecomputeBRDFSize;
		uint32_t m_MipMapLevels;
		TE_RESOURCE_FORMAT m_TextureIBLFormat;

		std::vector<RendererCommand> m_ContainerRendererCommand_Prefilter;
		RendererCommand m_RendererCommand_BRDF;

		PipelineGraphics m_PipelinePrefilter;
		PipelineGraphics m_PipelineBRDF;

		TextureRenderTarget* m_RenderTargetIBLSpecularPrefilter;
		TextureRenderTarget* m_RenderTargetIBLSpecularBRDF;

		std::vector<RenderTargetView> m_ContainerRTVPrefilter;
		RenderTargetView m_RTVBRDF;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		struct alignas(16) ConstantBuffer_Data
		{
			ConstantBuffer_Data()
			{}
			ConstantBuffer_Data(float _Roughness)
				: Roughness(_Roughness)
			{}

			float Roughness = 0.0f;
			float3 _Pad;
		};

		ConstantBufferDirect<ConstantBuffer_Data>* m_ConstantBufferDirect;
	};
}
