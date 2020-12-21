#pragma once
#include "CommandList.h"

namespace TruthEngine
{

	namespace API::DirectX12
	{
		class DirectX12CommandList;
	}

	namespace Core
	{
		class GraphicResource;
		class Shader;
		class ConstantBufferUploadBase;
		class ShaderManager;
		class Mesh;
		class Viewport;
		class ViewRect;

		class RendererCommand
		{

		public:
			RendererCommand();

			void Init(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint32_t ParallelCommandsNum = 1, std::shared_ptr<BufferManager> bufferManager = nullptr, std::shared_ptr<ShaderManager> shaderManager = nullptr);


			TextureRenderTarget* CreateRenderTarget(TE_IDX_RENDERTARGET idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource);
			TextureDepthStencil* CreateDepthStencil(TE_IDX_DEPTHSTENCIL idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource);
			TE_RESULT CreateResource(BufferUpload* cb);

			void CreateRenderTargetView(TE_IDX_RENDERTARGET idx, RenderTargetView* RTV);
			void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV);
			void CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* RTV);

			void CreateDepthStencilView(TE_IDX_DEPTHSTENCIL idx, DepthStencilView* DSV);
			void CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* DSV);


			void CreateShaderResourceView(Texture* textures[], uint32_t textureNum, ShaderResourceView* SRV);
			void CreateShaderResourceView(Texture* texture, ShaderResourceView* SRV);
			void CreateConstantBufferView(TE_IDX_CONSTANTBUFFER idx, ConstantBufferView* CBV);

			TE_RESULT CreateVertexBuffer(VertexBufferBase* vb);
			TE_RESULT CreateIndexBuffer(IndexBuffer* ib);


			void SetPipeline(Pipeline* pipeline, uint32_t cmdListIndex = 0);
			void SetRenderTarget(const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void SetDepthStencil(const DepthStencilView DSV, uint32_t cmdListIndex = 0);
			void SetShaderResource(const ShaderResourceView SRV, uint32_t registerIndex, uint32_t cmdListIndex = 0);
			void SetConstantBuffer(const ConstantBufferView CBV, uint32_t registerIndex, uint32_t cmdListIndex = 0);

			void UploadData(ConstantBufferUploadBase* cb, uint32_t cmdListIndex = 0);
			void UploadData(Buffer* buffer, void* Data, size_t sizeInByte, uint32_t cmdListIndex = 0);
			void UploadData(VertexBufferBase* vertexBuffer, uint32_t cmdListIndex = 0);
			void UploadData(IndexBuffer* indexBuffer, uint32_t cmdListIndex = 0);

			void SetVertexBuffer(VertexBufferBase* vertexBuffer, uint32_t cmdListIndex = 0);
			void SetIndexBuffer(IndexBuffer* indexBuffer, uint32_t cmdListIndex = 0);
			
			void SetViewPort(Viewport* viewport, ViewRect* rect, uint32_t cmdListIndex = 0);

			void Begin(uint32_t cmdListIndex = 0);
			void Begin(Pipeline* pipeline, uint32_t cmdListIndex = 0);
			void End(uint32_t cmdListIndex = 0);
			
			void EndAndPresent(uint32_t cmdListIndex = 0);

			void DrawIndexed(Mesh* mesh, uint32_t cmdListIndex = 0);
			void Draw(Mesh* mesh, uint32_t cmdListIndex = 0);

			void ClearRenderTarget(const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void ClearRenderTarget(const SwapChain* swapChain, const RenderTargetView RTV, uint32_t cmdListIndex = 0);
			void ClearDepthStencil(const DepthStencilView DSV, uint32_t cmdListIndex = 0);

			void Resize(TextureRenderTarget* texture, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);
			void Resize(TextureDepthStencil* texture, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV);

			bool IsRunning(uint32_t cmdListIndex = 0);
		private:



		private:
			std::vector<std::shared_ptr<CommandList>> m_CommandLists;


			std::shared_ptr<BufferManager> m_BufferManager;
			std::shared_ptr<ShaderManager> m_ShaderManager;

		};
	}
}