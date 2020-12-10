#pragma once
#include "CommandList.h"

namespace TruthEngine
{

	namespace API::DirectX12
	{
		class DX12CommandList;
	}

	namespace Core
	{
		class GraphicResource;
		class Shader;
		class ConstantBufferUploadBase;
		class ShaderManager;
		class Mesh;

		class RendererCommand
		{

		public:
			RendererCommand();

			void Init(uint32_t ParallelCommandsNum = 1, std::shared_ptr<BufferManager> bufferManager = nullptr, std::shared_ptr<ShaderManager> shaderManager = nullptr);

			void SetPipeline(Pipeline* pipeline, uint32_t cmdListIndex = 0);
			void SetRenderTarget(const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void SetDepthStencil(const DepthStencilView DSV, uint32_t cmdListIndex = 0);
			void SetShaderResource(const ShaderResourceView SRV, uint32_t registerIndex, uint32_t cmdListIndex = 0);
			void SetConstantBuffer(const ConstantBufferView CBV, uint32_t registerIndex, uint32_t cmdListIndex = 0);

			void UpdateConstantBuffer(ConstantBufferUploadBase* cb, uint32_t cmdListIndex = 0);

//			void ChangeResourceState(GraphicResource* resource, TE_RESOURCE_STATES newState, uint32_t cmdListIndex = 0);

			void SetVertexBuffer(VertexBufferBase* vertexBuffer, uint32_t cmdListIndex = 0);
			void SetIndexBuffer(IndexBuffer* indexBuffer, uint32_t cmdListIndex = 0);
			
			void Begin(uint32_t cmdListIndex = 0);
			void Begin(Pipeline* pipeline, uint32_t cmdListIndex = 0);
			void End(uint32_t cmdListIndex = 0);
			
			void EndAndPresent(uint32_t cmdListIndex = 0);

			void DrawIndexed(Mesh* mesh, uint32_t cmdListIndex = 0);
			void Draw(Mesh* mesh, uint32_t cmdListIndex = 0);

			void ClearRenderTarget(const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void ClearRenderTarget(const SwapChain* swapChain, const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void ClearDepthStencil(const DepthStencilView DSV, uint32_t cmdListIndex = 0);


		private:



		private:
			std::vector<std::shared_ptr<CommandList>> m_CommandLists;


			std::shared_ptr<BufferManager> m_BufferManager;
			std::shared_ptr<ShaderManager> m_ShaderManager;

		};
	}
}