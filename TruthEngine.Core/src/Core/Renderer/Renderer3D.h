#pragma once

#include "RendererCommand.h"
#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"
#include "BufferManager.h"
#include "ShaderManager.h"

namespace TruthEngine
{
	namespace Core
	{

		class Model3D;
		class ICamera;
		class Material;

		class Renderer3D
		{

		public:

			Renderer3D();
			~Renderer3D();

			Renderer3D(const Renderer3D& renderer3D);
			Renderer3D& operator=(const Renderer3D& renderer3D);


			void Init(BufferManager* bufferMgr, const std::vector<Material>& materials);

			void BeginScene();
			void EndScene();

			void Render(std::vector<const Model3D*> model);


		private:



		private:

			RendererCommand m_RendererCommand;
			TextureDepthStencil m_TextureDepthStencil;

			RenderTargetView m_RenderTartgetView;
			DepthStencilView m_DepthStencilView;

			std::shared_ptr<ShaderManager> m_ShaderMgr;

			BufferManager* m_BufferMgr;

			std::unordered_map<uint32_t, std::shared_ptr<Pipeline>> m_MaterialPipelines;

		};
	}
}
