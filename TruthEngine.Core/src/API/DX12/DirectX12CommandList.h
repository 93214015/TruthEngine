#pragma once

#include "Core/Renderer/CommandList.h"

#include "DirectX12CommandAllocator.h"


namespace TruthEngine
{

	class BufferUploadIntermediate;

	namespace API::DirectX12
	{

		class DirectX12BufferManager;
		class DirectX12ShaderManager;
		class DirectX12PiplineManager;
		class DirectX12RootArguments;


		class DirectX12CommandList : public CommandList
		{

		public:
			DirectX12CommandList(GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, BufferManager* bufferManager
				, ShaderManager* shaderManager, TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint8_t frameIndex);


			void Release() override;


			inline ID3D12GraphicsCommandList* GetNativeObject() const
			{
				return m_D3D12CommandList.Get();
			}


			inline DirectX12CommandAllocator* GetCommandAllocator()
			{
				return &m_CommandAllocator;
			}


			void Reset() override;


			void Reset(Pipeline* pipeline) override;


			void SetPipeline(Pipeline* pipeline) override;


			//			void SetRenderTarget(TextureRenderTarget* RenderTargets[], const uint32_t RenderTargetNum, const TextureDepthStencil* DepthStencil) override;
			void SetRenderTarget(const RenderTargetView RTV) override;
			void SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV) override;


			void SetDepthStencil(const DepthStencilView DSV);


			void SetShaderResource(const ShaderResourceView srv, const uint32_t registerIndex) override;


			void SetConstantBuffer(const ConstantBufferView cbv, const uint32_t registerIndex) override;


			void UpdateConstantBuffer(ConstantBufferUploadBase* cb) override;


			void UploadData(Buffer* buffer, const void* data, size_t sizeInByte) override;


			void UploadData(ConstantBufferDirectBase* cb) override;


			void SetVertexBuffer(VertexBufferBase* vertexBuffer) override;


			void SetIndexBuffer(IndexBuffer* indexBuffer) override;


			void DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset) override;


			void Draw(uint32_t vertexNum, uint32_t vertexOffset) override;


			void ClearRenderTarget(const RenderTargetView RTV);
			void ClearRenderTarget(const SwapChain* swapChain, const RenderTargetView RTV);
			void ClearDepthStencil(const DepthStencilView DSV);


			void SetViewport(Viewport* viewport, ViewRect* rect);


			void Submit() override;


			void Commit() override;


			void Present() override;


			bool IsRunning() override;


			void WaitToFinish() override;

		protected:


			void _BindResource();

			void _ChangeResourceState(GraphicResource* resource, TE_RESOURCE_STATES newState);


			inline void _QueueBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
			{
				m_ResourceBarriers.emplace_back(
					CD3DX12_RESOURCE_BARRIER::Transition(
						resource
						, before
						, after
					)
				);
			}


			void _UploadDefaultBuffers();


			void _ResetContainers();


			void _SetDescriptorHeapSRV();



		protected:


			struct ClearingRenderTarget
			{
				ClearingRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, ClearValue_RenderTarget clearValue, D3D12_RECT rect) : RTV(rtv), ClearValue(clearValue), mRect(rect)
				{}

				D3D12_CPU_DESCRIPTOR_HANDLE RTV;
				ClearValue_RenderTarget ClearValue;
				D3D12_RECT mRect;
			};


			struct ClearingDepthStencil
			{
				ClearingDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsv, ClearValue_DepthStencil clearValue, D3D12_RECT rect)
					: DSV(dsv), ClearValue(clearValue), mRect(rect)
				{}

				D3D12_CPU_DESCRIPTOR_HANDLE DSV;
				ClearValue_DepthStencil ClearValue;
				D3D12_RECT mRect;
			};


			struct CopyPending_DefaultResource {

				CopyPending_DefaultResource(ID3D12Resource* resource, const void* data, size_t size)
					: D3D12Resource(resource), Data(data), Size(size)
				{}

				ID3D12Resource* D3D12Resource;
				const void* Data;
				size_t Size;
			};


			COMPTR<ID3D12GraphicsCommandList> m_D3D12CommandList;

			DirectX12CommandAllocator m_CommandAllocator;

			DirectX12BufferManager* m_BufferManager;

			DirectX12ShaderManager* m_ShaderManager;

			std::vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;

			std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SRVHandles_CB;

			std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SRVHandles_Texture;

			std::vector<ClearingRenderTarget> m_QueueClearRT;

			std::vector<ClearingDepthStencil> m_QueueClearDS;

			std::vector<CopyPending_DefaultResource> m_QueueCopyDefaultResource;

			D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;

			D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandles[8] = { D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE() };

			uint32_t m_RTVHandleNum = 0;

			uint32_t m_DSVHandleNum = 0;

			size_t m_CopyQueueRequiredSize = 0;

			COMPTR<ID3D12Resource> m_IntermediateResource;

			const DirectX12RootArguments* m_RootArguments = nullptr;

			//
			// friend class
			//
			friend class DirectX12CommandQueue;
			friend class DirectX12CommandQueue_Direct;
			friend class DirectX12CommandQueue_Copy;

		};


	}
}
