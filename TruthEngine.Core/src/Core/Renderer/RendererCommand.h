#pragma once
#include "CommandList.h"

namespace TruthEngine
{

	namespace API::DirectX12
	{
		class DirectX12CommandList;
	}

	namespace Profiler
	{
		class GPUEvent;
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

		TextureRenderTarget* CreateRenderTarget(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, uint8_t mipLevels, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA);
		TextureRenderTarget* CreateRenderTargetArray(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, uint8_t arraySize, uint8_t mipLevels, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA);
		TextureRenderTarget* CreateRenderTargetCubeMap(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, uint8_t mipLevels, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA);
		TextureDepthStencil* CreateDepthStencil(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, uint8_t mipLevels, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource, bool enbaleMSAA);
		TextureCubeMap* CreateTextureCubeMap(TE_IDX_GRESOURCES idx, const char* filePath);
		Texture* CreateTexture(TE_IDX_GRESOURCES _IDX, uint32_t _Width, uint32_t _Height, uint8_t _ArraySize, uint8_t _MipLevels, TE_RESOURCE_FORMAT _Format, TE_RESOURCE_TYPE _ResourceType, TE_RESOURCE_STATES _State, const void* _InitData);
		Texture* LoadTextureFromFile(TE_IDX_GRESOURCES _IDX, const char* _FilePath, uint8_t _MipLevels = 1);
		TextureMaterial* LoadTextureMaterialFromFile(const char* _FilePath);
		void SaveTextureToFile(const Texture& _Textue, const char* _FilePath);
		void SaveTextureToFile(TE_IDX_GRESOURCES _TextureIDX, const char* _FilePath);
		Buffer* CreateBufferStructuredRW(TE_IDX_GRESOURCES _IDX, uint32_t _ElementSizeInByte, uint32_t _ElementNum, bool _IsByteAddressBuffer);
		Texture* CreateTextureRW(TE_IDX_GRESOURCES _IDX, uint32_t _Width, uint32_t _Height, TE_RESOURCE_FORMAT _Format, bool _UseAsShaderResource, bool _EnableMSAA);

		template<class T> ConstantBufferUpload<T>* CreateConstantBufferUpload(TE_IDX_GRESOURCES idx)
		{
			return m_BufferManager->CreateConstantBufferUpload<T>(idx);
		}
		template<class T> ConstantBufferDirect<T>* CreateConstantBufferDirect(TE_IDX_GRESOURCES idx)
		{
			return m_BufferManager->CreateConstantBufferDirect<T>(idx);
		}
		template<class T> ConstantBufferUpload<T>* GetConstantBufferUpload(TE_IDX_GRESOURCES idx)
		{
			return static_cast<ConstantBufferUpload<T>*>(m_BufferManager->GetConstantBufferUpload(idx));
		}

		TE_RESULT CreateResource(BufferUpload* cb);

		void ReleaseResource(GraphicResource* graphicResource);
		void ReleaseResource(TE_IDX_GRESOURCES idx);

		void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV);
		void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV, uint8_t MipSlice, uint8_t ArraySlice);
		void CreateRenderTargetView(TE_IDX_GRESOURCES idx, RenderTargetView* RTV);
		void CreateRenderTargetView(TE_IDX_GRESOURCES idx, RenderTargetView* RTV, uint8_t MipSlice, uint8_t ArraySlice);
		void CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* RTV);

		void CreateDepthStencilView(TE_IDX_GRESOURCES idx, DepthStencilView* DSV);
		void CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* DSV);

		void CreateShaderResourceView(Texture* texture, ShaderResourceView* SRV);
		void CreateConstantBufferView(TE_IDX_GRESOURCES idx, ConstantBufferView* CBV);

		TE_RESULT CreateVertexBuffer(VertexBufferBase* vb);
		TE_RESULT CreateIndexBuffer(IndexBuffer* ib);

		TextureRenderTarget* GetRenderTarget(TE_IDX_GRESOURCES _IDX);
		TextureDepthStencil* GetDepthStencil(TE_IDX_GRESOURCES _IDX);

		void SetPipelineGraphics(PipelineGraphics* pipeline);
		void SetPipelineCompute(PipelineCompute* pipeline);
		void SetRenderTarget(const RenderTargetView& RTV);
		void SetRenderTarget(SwapChain* swapChain, const RenderTargetView& RTV);
		void SetDepthStencil(const DepthStencilView& DSV);
		void SetDepthStencil(const DepthStencilView& DSV, uint32_t _StencilRefValue);
		void SetDirectConstantGraphics(ConstantBufferDirectBase* cb);
		void SetDirectConstantCompute(ConstantBufferDirectBase* cb);
		void SetDirectShaderResourceViewGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot);
		void SetDirectShaderResourceViewCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot);
		void SetDirectConstantBufferViewGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot);
		void SetDirectConstantBufferViewCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot);
		void SetDirectUnorderedAccessViewGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot);
		void SetDirectUnorderedAccessViewCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot);
		void SetDirectShaderResourceViewGraphics(TE_IDX_GRESOURCES _GraphicResourceIDX, uint32_t _ShaderRegisterSlot);
		void SetDirectShaderResourceViewCompute(TE_IDX_GRESOURCES _GraphicResourceIDX, uint32_t _ShaderRegisterSlot);
		void SetDirectConstantBufferViewGraphics(TE_IDX_GRESOURCES _GraphicResourceIDX, uint32_t _ShaderRegisterSlot);
		void SetDirectConstantBufferViewCompute(TE_IDX_GRESOURCES _GraphicResourceIDX, uint32_t _ShaderRegisterSlot);
		void SetDirectUnorderedAccessViewGraphics(TE_IDX_GRESOURCES _GraphicResourceIDX, uint32_t _ShaderRegisterSlot);
		void SetDirectUnorderedAccessViewCompute(TE_IDX_GRESOURCES _GraphicResourceIDX, uint32_t _ShaderRegisterSlot);
		/*void SetShaderResource(const ShaderResourceView SRV, uint32_t registerIndex);
		void SetConstantBuffer(const ConstantBufferView CBV, uint32_t registerIndex);*/

		void UploadData(ConstantBufferUploadBase* cb);
		void UploadData(Buffer* buffer, void* Data, size_t sizeInByte);
		void UploadData(VertexBufferBase* vertexBuffer);
		void UploadData(IndexBuffer* indexBuffer);

		void CopyResource(GraphicResource* _Source, GraphicResource* _Dest);
		void CopyResource(TE_IDX_GRESOURCES _SourceIDX, GraphicResource* _Dest);

		void SetVertexBuffer(VertexBufferBase* vertexBuffer);
		void SetIndexBuffer(IndexBuffer* indexBuffer);

		void SetViewPort(const Viewport* viewport, const ViewRect* rect);

		void BeginCompute();
		void BeginCompute(PipelineCompute* pipeline);
		void BeginGraphics();
		void BeginGraphics(PipelineGraphics* pipeline);

		void ExecutePendingCommands();
		void End();
		void EndAndPresent();

		void DrawIndexed(const Mesh* mesh);
		void Draw(Mesh* mesh);
		void Draw(uint32_t _VertexNum, uint32_t _VertexOffset);

		void Dispatch(uint32_t GroupNumX, uint32_t GroupNumY, uint32_t GroupNumZ);

		void ClearRenderTarget(const RenderTargetView& RTV);
		void ClearRenderTarget(SwapChain* swapChain, const RenderTargetView& RTV);
		void ClearDepthStencil(const DepthStencilView& DSV);

		void ResizeRenderTarget(TextureRenderTarget* texture, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);
		void ResizeRenderTarget(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);
		void ResizeDepthStencil(TextureDepthStencil* texture, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV);
		void ResizeDepthStencil(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV);
		void ResizeSwapChain(SwapChain* swapChain, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV);

		void ResolveMultiSampledTexture(Texture* _SourceTexture, Texture* _DestTexture);
		void ResolveMultiSampledTexture(Texture* _SourceTexture, TE_IDX_GRESOURCES _TextureIDX);

		void AddUpdateTask(const std::function<void()>& task);
		void AddUpdateTaskJustCurrentFrame(const std::function<void()>& task);

		bool IsRunning();

	private:
		void WaitForGPU();

	private:

		std::vector<std::shared_ptr<CommandList>> m_CommandLists;
		CommandList* m_CurrentCommandList = nullptr;

		BufferManager* m_BufferManager;
		ShaderManager* m_ShaderManager;

		friend class Profiler::GPUEvent;

	};
}