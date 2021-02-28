#pragma once
#include "BufferManager.h"
#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"

namespace TruthEngine
{

	class GraphicDevice;
	class Pipeline;
	class GraphicResource;
	class TextureRenderTarget;
	class TextureDepthStencil;
	class Texture;
	class ConstantBufferUploadBase;
	class VertexBufferBase;
	class IndexBuffer;
	class ShaderManager;
	class BufferUplaod;
	class ShaderSignature;

	struct ViewRect;
	struct Viewport;


	class CommandList 
	{

	public:
		CommandList(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint8_t frameIndex);
		virtual ~CommandList() = default;

		virtual void Reset() = 0;
		virtual void Reset(Pipeline* pipeline) = 0;

		virtual void SetPipeline(Pipeline* pipeline) = 0;
//		virtual void SetRenderTarget(TextureRenderTarget* RenderTargets[], const uint32_t RenderTargetNum, const TextureDepthStencil* DepthStencil) = 0;
		virtual void SetRenderTarget(const RenderTargetView RTV) = 0;
		virtual void SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV) = 0;
		virtual void SetDepthStencil(const DepthStencilView DSV) = 0;
		virtual void SetShaderResource(ShaderResourceView srv, const uint32_t registerIndex) = 0;
		virtual void SetConstantBuffer(ConstantBufferView cbv, const uint32_t registerIndex) = 0;

		virtual void UpdateConstantBuffer(ConstantBufferUploadBase* cb) = 0;

		virtual void UploadData(Buffer* buffer, const void* data, size_t sizeInByte) = 0;
		virtual void UploadData(ConstantBufferDirectBase* cb) = 0;

		virtual void SetVertexBuffer(VertexBufferBase* vertexBuffer) = 0;
		virtual void SetIndexBuffer(IndexBuffer* indexBuffer) = 0;

		virtual void ClearRenderTarget(const RenderTargetView RTV) = 0;
		virtual void ClearRenderTarget(const SwapChain* swapChain, const RenderTargetView RTV) = 0;
		virtual void ClearDepthStencil(const DepthStencilView DSV) = 0;

		virtual void SetViewport(Viewport* viewport, ViewRect* rect) = 0;

		virtual void DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset) = 0;
		virtual void Draw(uint32_t vertexNum, uint32_t vertexOffset) = 0;

		virtual void Release() = 0;

		virtual void Submit() = 0;
		virtual void Commit() = 0;

		virtual void Present() = 0;

		virtual bool IsRunning() = 0;

		virtual void WaitToFinish() = 0;

		inline void AddUpdateTask(const std::function<void()>& task)
		{
			m_UpdateQueue.emplace_back(task);
		}

		inline void ExecuteUpdateTasks()
		{
			if (m_UpdateQueue.size() > 0)
			{
				for (auto& task : m_UpdateQueue)
					task();

				m_UpdateQueue.clear();
			}
		}


		static std::shared_ptr<CommandList> Factory(GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, BufferManager* bufferManager, ShaderManager* shaderManager, TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint8_t frameIndex);

	protected:
		CommandList() = default;

	protected:

		std::vector<std::function<void()>> m_UpdateQueue;

		uint8_t m_FrameIndex = 0;

		uint32_t m_AssignedVertexBufferID = -1;
		uint32_t m_AssignedIndexBufferID = -1;

		ShaderSignature* m_ShaderSignature;

		TE_IDX_SHADERCLASS m_ShaderClassIDX = TE_IDX_SHADERCLASS::NONE;
		TE_IDX_RENDERPASS m_RenderPassIDX = TE_IDX_RENDERPASS::NONE;

	};

}
