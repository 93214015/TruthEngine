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

		struct Viewport;
		struct ViewRect;

		class RendererCommand
		{

		public:
			RendererCommand();

			void Init(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint32_t ParallelCommandsNum = 1, std::shared_ptr<BufferManager> bufferManager = nullptr, std::shared_ptr<ShaderManager> shaderManager = nullptr);
			void Release();

			TextureRenderTarget* CreateRenderTarget(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource);
			TextureDepthStencil* CreateDepthStencil(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource);

			template<class T> ConstantBufferUpload<T>* CreateConstantBufferUpload(TE_IDX_CONSTANTBUFFER idx)
			{
				return m_BufferManager->CreateConstantBufferUpload<T>(idx);
			}

			template<class T> ConstantBufferUploadArray<T>* CreateConstantBufferUploadArray(TE_IDX_CONSTANTBUFFER idx, uint32_t arraySize)
			{
				return m_BufferManager->CreateConstantBufferUploadArray<T>(idx, arraySize);
			}

			template<class T> ConstantBufferDirect<T>* CreateConstantBufferDirect(TE_IDX_CONSTANTBUFFER idx)
			{
				return m_BufferManager->CreateConstantBufferDirect<T>(idx);
			}

			template<class T> ConstantBufferUpload<T>* GetConstantBufferUpload(TE_IDX_CONSTANTBUFFER idx)
			{
				return static_cast<ConstantBufferUpload<T>*>(m_BufferManager->GetConstantBufferUpload(idx));
			}

			TE_RESULT CreateResource(BufferUpload* cb);

			void ReleaseResource(GraphicResource* graphicResource);
			void ReleaseResource(TE_IDX_TEXTURE idx);

			void CreateRenderTargetView(TE_IDX_TEXTURE idx, RenderTargetView* RTV);
			void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV);
			void CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* RTV);

			void CreateDepthStencilView(TE_IDX_TEXTURE idx, DepthStencilView* DSV);
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
			void UploadData(ConstantBufferDirectBase* cb, uint32_t cmdListIndex = 0);
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

			void ResizeRenderTarget(TextureRenderTarget* texture, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);
			void ResizeRenderTarget(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);
			void ResizeDepthStencil(TextureDepthStencil* texture, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV);
			void ResizeDepthStencil(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV);
			void ResizeSwapChain(SwapChain* swapChain, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);

			bool IsRunning(uint32_t cmdListIndex = 0);

		private:
			void WaitForGPU();

		private:
			uint32_t m_LastCommadListIndex = 0;

			std::vector<std::shared_ptr<CommandList>> m_CommandLists;

			std::shared_ptr<BufferManager> m_BufferManager;
			std::shared_ptr<ShaderManager> m_ShaderManager;

		};
	}
}