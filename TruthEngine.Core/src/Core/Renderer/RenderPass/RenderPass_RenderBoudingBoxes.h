#pragma once
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/Pipeline.h"


namespace TruthEngine
{

	class RenderPass_RenderBoundingBoxes final : public RenderPass
	{

	public:
		RenderPass_RenderBoundingBoxes(RendererLayer* _RendererLayer);


		void OnAttach() override;


		void OnDetach() override;


		void OnImGuiRender() override;


		void OnUpdate(double _DeltaTime) override;


		void BeginScene() override;


		void EndScene() override;


		void Render() override;


	private:
		void InitTextures();
		void InitBuffers();
		void InitPipelines();
		void RegisterOnEvents();

		void OnEventRendererViewportResize(class EventRendererViewportResize& _Event);

	private:
		RendererCommand m_RendererCommand;

		RenderTargetView m_RenderTargetView;
		DepthStencilView m_DepthStencilView;

		PipelineGraphics m_Pipeline;

		struct alignas(16) ConstantBuffer_Data_PerBoundingBox
		{
			ConstantBuffer_Data_PerBoundingBox() = default;
			ConstantBuffer_Data_PerBoundingBox(const float3& _Extents, const float3& _Center)
				: Extents(_Extents), Center(_Center)
			{}


			float3 Extents;
			float _pad0 = 0.0f;
			float3 Center;
			float _pad1 = 0.0f;
		};

		ConstantBufferDirect<ConstantBuffer_Data_PerBoundingBox>* m_ConstantBufferDirect_PerBB;
	};

}
