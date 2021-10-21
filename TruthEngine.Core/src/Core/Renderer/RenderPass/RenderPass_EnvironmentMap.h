#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{
	class RenderPass_EnvironmentMap : public RenderPass
	{
	public:
		RenderPass_EnvironmentMap(RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		virtual void OnImGuiRender() override;
		virtual void OnUpdate(double _DeltaTime) override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void Render() override;

	private:
		virtual void InitRendererCommand() override;
		virtual void InitTextures() override;
		virtual void InitBuffers() override;
		virtual void InitPipelines() override;
		virtual void ReleaseRendererCommand() override;
		virtual void ReleaseTextures() override;
		virtual void ReleaseBuffers() override;
		virtual void ReleasePipelines() override;
		virtual void RegisterEventListeners() override;
		virtual void UnRegisterEventListeners() override;

	private:
		RendererCommand m_RendererCommand;

		const RenderTargetView* m_RTV;
		const DepthStencilView* m_DSV;

		PipelineGraphics m_PipelineEnvironmentMap;

		ConstantBufferDirect<ConstantBuffer_Data_EnvironmentMap>* m_ConstantBufferDirect_EnvironmentMap;
	};
}
