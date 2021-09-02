#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{

	class RenderPass_Wireframe final : public RenderPass
	{
	public:
		RenderPass_Wireframe(class RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		void OnAttach() override;

		void OnDetach() override;

		void OnImGuiRender() override;

		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;

		void EndScene() override;

		void Render(const Mesh* _Mesh, const float4x4A& _Transform);

	private:
		void Render() override {};

		void InitRendererCommand();
		void InitTextures();
		void InitBuffers();
		void InitPipelines();

		void ReleaseTextures();
		void ReleaseBuffers();
		void ReleasePipelines();
		void ReleaseRendererCommand();

		void RegiterOnEvents();

		void OnEventRendererViewportResize(class EventRendererViewportResize& _Event);

	private:

		RendererCommand m_RendererCommand;

		RenderTargetView m_RTV;
		DepthStencilView m_DSV;

		PipelineGraphics m_Pipeline;

		struct alignas(16) ConstantBufferData_Wireframe
		{
			float4x4A WVP;
		};

		ConstantBufferDirect<ConstantBufferData_Wireframe>* m_ConstantBuffer;

	};

}
