#pragma once

#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{

	class RenderPass_GenerateCubeMap final : public RenderPass
	{
	public:
		RenderPass_GenerateCubeMap(RendererLayer* _RendererLayer);

		void OnImGuiRender() override;
		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;
		void EndScene() override;

		void Render() override;

		void Initialize(TE_IDX_GRESOURCES _TextureCubeIDX, size_t _CubeMapSize, TE_RESOURCE_FORMAT _TextureCubeFormat, const char* _FilePath);

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

		bool m_IsInitialized = false;
		TE_IDX_GRESOURCES m_TextureCubeIDX = TE_IDX_GRESOURCES::NONE;
		TE_RESOURCE_FORMAT m_TextureCubeFormat = TE_RESOURCE_FORMAT::UNKNOWN;
		size_t m_CubeMapSize = 1024;

		std::string m_InputTextureFilePath = "";

		RendererCommand m_RendererCommand_GenerateCubeMap;
		//RendererCommand m_RendererCommand_GenerateIBL;

		RenderTargetView m_RenderTartgetViewCubeMap;
		//RenderTargetView m_RenderTartgetViewIBL;

		TextureRenderTarget* m_TextureRenderTargetCubeMap = nullptr;
		//TextureRenderTarget* m_TextureRenderTargetIBL = nullptr;
		Texture* m_TextureEnvironment;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		class MaterialManager* m_MaterialManager;

		PipelineGraphics m_PipelineGenerateCubeMap;
		//PipelineGraphics m_PipelineGenerateIBL;
	};
}