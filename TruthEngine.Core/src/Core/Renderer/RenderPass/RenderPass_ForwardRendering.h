#pragma once

#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/TextureDepthStencil.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/Viewport.h"

namespace TruthEngine
{
	namespace Core
	{
		class Model3D;
		class ICamera;
		class Material;



		class RenderPass_ForwardRendering : public RenderPass
		{

		public:

			RenderPass_ForwardRendering();
			~RenderPass_ForwardRendering();

			RenderPass_ForwardRendering(const RenderPass_ForwardRendering& renderer3D);
			RenderPass_ForwardRendering& operator=(const RenderPass_ForwardRendering& renderer3D);

			void OnAttach() override;
			void OnDetach() override;

			void OnImGuiRender() override;

			void BeginScene();
			void EndScene();

			void Render(std::vector<const Model3D*> model);

			void OnSceneViewportResize(uint32_t width, uint32_t height) override;
		private:



		private:

			RendererCommand m_RendererCommand;
			TextureDepthStencil* m_TextureDepthStencil;

			RenderTargetView m_RenderTartgetView;
			DepthStencilView m_DepthStencilView;

			Viewport m_Viewport;
			ViewRect m_ViewREct;

			std::shared_ptr<ShaderManager> m_ShaderMgr;

			BufferManager* m_BufferMgr;

			std::unordered_map<uint32_t, std::shared_ptr<Pipeline>> m_MaterialPipelines;

			struct CB_DATA_PER_MESH
			{
				float4 color;
			};

			ConstantBufferDirect<CB_DATA_PER_MESH> m_ConstantBufferDirect_PerMesh;

		};
	}
}
