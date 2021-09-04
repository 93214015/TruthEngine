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

		void OnImGuiRender() override;

		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;

		void EndScene() override;

		void Render() override;

		struct QueueItem
		{
			QueueItem(const float4x4A& _Transform, const float4& _Color, const Mesh* _Mesh)
				: Transform(_Transform), Color(_Color), Mesh(_Mesh)
			{}

			float4x4A Transform;
			float4 Color;
			const Mesh* Mesh;
		};

		void Queue(const QueueItem& _QueueItem);



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

		RendererCommand m_RendererCommand;

		PipelineGraphics m_Pipeline;

		struct alignas(16) ConstantBufferData_Wireframe
		{
			ConstantBufferData_Wireframe() = default;
			ConstantBufferData_Wireframe(const float4x4A& _WVP, const float4& _Color)
				: WVP(_WVP), Color(_Color)
			{}

			float4x4A WVP;
			float4 Color;
		};

		ConstantBufferDirect<ConstantBufferData_Wireframe>* m_ConstantBuffer;


		std::vector<QueueItem> m_Queue;

	};

}
