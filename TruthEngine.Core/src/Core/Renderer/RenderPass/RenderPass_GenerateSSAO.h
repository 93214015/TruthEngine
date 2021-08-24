#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ConstantBuffer.h"


namespace TruthEngine
{
	class RenderPass_GenerateSSAO : public RenderPass
	{
	public:


		// Inherited via RenderPass
		virtual void OnAttach() override;

		virtual void OnDetach() override;

		virtual void OnImGuiRender() override;

		virtual void OnUpdate(double _DeltaTime) override;

		virtual void BeginScene() override;

		virtual void EndScene() override;

		virtual void Render() override;

	private:

		void InitTextures();
		void InitBuffers();
		void ReleaseTextures();
		void ReleaseBuffers();

	private:

		RendererCommand m_RendererCommand;

		TextureRenderTarget* m_TextureRenderTargetSSAO;

		Texture* m_TextureRandomVectors;

		struct ConstantBufferData_SSAO
		{
			float3A KernelSamples[64];
		};

		ConstantBufferUpload<ConstantBufferData_SSAO>* m_ConstantBufferSSAO;

	};
}
