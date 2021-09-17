#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/BufferManager.h"

namespace TruthEngine
{

	class RenderPass_AmbientReflection final : public RenderPass
	{
	public:
		RenderPass_AmbientReflection(class RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		void OnImGuiRender() override;

		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;

		void EndScene() override;

		void Render() override;

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

		void OnEventRendererViewportResize(class EventRendererViewportResize& _Event);

	private:

		RendererCommand m_RendererCommand_CopyResource;
		RendererCommand m_RendererCommand_Blend;
		RendererCommand m_RendererCommand_BlurHorz;
		RendererCommand m_RendererCommand_BlurVert;


		PipelineGraphics m_Pipeline_Blend;
		PipelineCompute m_Pipeline_BlurHorz;
		PipelineCompute m_Pipeline_BlurVert;

		Texture* m_Texture_SceneBufferBlur;
		Texture* m_Texture_SceneBufferBlur_Temp;
		Texture* m_Texture_SceneBuffer;

		struct ConstantBufferData_Blur
		{
			uint2 InputResolution;
			uint2 _pad;
		};
		ConstantBufferDirect<ConstantBufferData_Blur>* m_ConstantBuffer_Blur;


		std::vector<EventListenerHandle> m_EventListenerList;



	};

}
