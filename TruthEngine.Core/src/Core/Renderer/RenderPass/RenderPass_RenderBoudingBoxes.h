#pragma once
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/VertexBuffer.h"


namespace TruthEngine
{

	class RenderPass_RenderBoundingBoxes : public RenderPass
	{

	public:
		RenderPass_RenderBoundingBoxes(RendererLayer* _RendererLayer);


		void OnAttach() override;


		void OnDetach() override;


		void OnImGuiRender() override;


		void BeginScene() override;


		void EndScene() override;


		void Render() override;


	private:


	private:
		RendererCommand m_RendererCommand;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		RenderTargetView m_RenderTargetView;
		DepthStencilView m_DepthStencilView;


		struct alignas(16) ConstantBuffer_Data_PerObject
		{

			ConstantBuffer_Data_PerObject(const float4x4A& _WorldTransform)
				: mWorldTransform(_WorldTransform)
			{}

			float4x4A mWorldTransform;

			float4 mExtents;
		};

		ConstantBufferDirect<ConstantBuffer_Data_PerObject>* m_ConstantBufferDirect_PerObject;
	};

}
