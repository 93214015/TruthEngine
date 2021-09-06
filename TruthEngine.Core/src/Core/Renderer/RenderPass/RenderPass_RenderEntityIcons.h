#pragma once

#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Event/EventRenderer.h"

namespace TruthEngine
{

	class RenderPass_RenderEntityIcons final : public RenderPass
	{
	public:
		RenderPass_RenderEntityIcons(class RendererLayer* _RendererLayer);

		// Inherited via RenderPass

		void OnImGuiRender() override;

		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;

		void EndScene() override;

		void Render() override;

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

		bool m_IsDrawListEmpty = true;

		RendererCommand m_RendererCommand;

		PipelineGraphics m_Pipeline;

		struct alignas(16) ConstantBufferData_RenderEntityIcon
		{
			float3 Center;
			uint32_t TextureIndex;
			float3 Extents;
			float _pad0;
		};

		ConstantBufferDirect<ConstantBufferData_RenderEntityIcon>* m_ConstantBuffer;

	};

}