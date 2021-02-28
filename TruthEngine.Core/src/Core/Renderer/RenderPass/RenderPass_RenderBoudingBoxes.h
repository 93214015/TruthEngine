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
		RenderPass_RenderBoundingBoxes();


		void OnAttach() override;


		void OnDetach() override;


		void OnImGuiRender() override;


		void BeginScene() override;


		void EndScene() override;


		void Render() override;


		void OnSceneViewportResize(uint32_t width, uint32_t height) override;


	private:


	private:
		RendererCommand m_RendererCommand;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		RenderTargetView m_RenderTargetView;
		DepthStencilView m_DepthStencilView;


		struct ConstantBuffer_Data_PerObject
		{

			ConstantBuffer_Data_PerObject(const float4x4& _WorldTransform)
				: mWorldTransform(_WorldTransform)
			{}

			float4x4 mWorldTransform;

			float4 mExtents;
		};

		ConstantBufferDirect<ConstantBuffer_Data_PerObject>* m_ConstantBufferDirect_PerObject;
	};

}
