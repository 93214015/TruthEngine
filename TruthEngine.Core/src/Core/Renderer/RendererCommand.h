#pragma once
#include "CommandList.h"

namespace TruthEngine
{

	namespace API::DirectX12
	{
		class DirectX12CommandList;
	}
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

			void Init(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, BufferManager* bufferManager = nullptr, ShaderManager* shaderManager = nullptr);
			void Release();

			TextureRenderTarget* CreateRenderTarget(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA);
			TextureDepthStencil* CreateDepthStencil(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource, bool enbaleMSAA);
			TextureCubeMap* CreateTextureCubeMap(TE_IDX_TEXTURE idx, const char* filePath);

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


			void SetPipeline(Pipeline* pipeline);
			void SetRenderTarget(const RenderTargetView RTV);
			void SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV);
			void SetDepthStencil(const DepthStencilView DSV);
			void SetShaderResource(const ShaderResourceView SRV, uint32_t registerIndex);
			void SetConstantBuffer(const ConstantBufferView CBV, uint32_t registerIndex);

			void UploadData(ConstantBufferUploadBase* cb);
			void UploadData(ConstantBufferDirectBase* cb);
			void UploadData(Buffer* buffer, void* Data, size_t sizeInByte);
			void UploadData(VertexBufferBase* vertexBuffer);
			void UploadData(IndexBuffer* indexBuffer);

			void SetVertexBuffer(VertexBufferBase* vertexBuffer);
			void SetIndexBuffer(IndexBuffer* indexBuffer);
			
			void SetViewPort(Viewport* viewport, ViewRect* rect);

			void Begin();
			void Begin(Pipeline* pipeline);
			
			void End();
			void EndAndPresent();

			void DrawIndexed(const Mesh* mesh);
			void Draw(Mesh* mesh);

			void ClearRenderTarget(const RenderTargetView RTV);
			void ClearRenderTarget(const SwapChain* swapChain, const RenderTargetView RTV);
			void ClearDepthStencil(const DepthStencilView DSV);

			void ResizeRenderTarget(TextureRenderTarget* texture, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);
			void ResizeRenderTarget(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);
			void ResizeDepthStencil(TextureDepthStencil* texture, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV);
			void ResizeDepthStencil(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV);
			void ResizeSwapChain(SwapChain* swapChain, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);

			void AddUpdateTask(const std::function<void()>& task);

			bool IsRunning();

		private:
			void WaitForGPU();

		private:

			std::vector<std::shared_ptr<CommandList>> m_CommandLists;
			CommandList* m_CurrentCommandList = nullptr;

			BufferManager* m_BufferManager;
			ShaderManager* m_ShaderManager;

		};
}