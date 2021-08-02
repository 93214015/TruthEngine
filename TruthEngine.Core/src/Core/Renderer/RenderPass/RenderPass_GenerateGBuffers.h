#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/BufferManager.h"

namespace TruthEngine
{
	class RenderPass_GenerateGBuffers : public RenderPass
	{
	public:

		RenderPass_GenerateGBuffers(class RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void Render() override;

	private:
		void InitTextures();


	private:
		RendererCommand m_RendererCommand;

		RenderTargetView m_RenderTargetViewGBufferColor;
		RenderTargetView m_RenderTargetViewGBufferNormal;

		TextureRenderTarget* m_TextureGBufferColor;
		TextureRenderTarget* m_TextureGBufferNormal;
	};
}
