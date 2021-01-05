#pragma once

#include "Core/Renderer/CommandList.h"

#include "DirectX12CommandAllocator.h"


namespace TruthEngine
{
	
	namespace Core {
		class BufferUploadIntermediate;
	}

	namespace API::DirectX12 
	{

		class DirectX12BufferManager;
		class DirectX12ShaderManager;
		class DirectX12PiplineManager;
		class DirectX12RootArguments;


		class DirectX12CommandList : public Core::CommandList
		{

		public:
			DirectX12CommandList(Core::GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, Core::BufferManager* bufferManager, Core::ShaderManager* shaderManager, TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX);


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


			void Reset(Core::Pipeline* pipeline) override;


			void SetPipeline(Core::Pipeline* pipeline) override;


//			void SetRenderTarget(Core::TextureRenderTarget* RenderTargets[], const uint32_t RenderTargetNum, const Core::TextureDepthStencil* DepthStencil) override;
			void SetRenderTarget(const Core::RenderTargetView RTV) override;
			void SetRenderTarget(Core::SwapChain* swapChain, const Core::RenderTargetView RTV) override;


			void SetDepthStencil(const Core::DepthStencilView DSV);


			void SetShaderResource(const Core::ShaderResourceView srv, const uint32_t registerIndex) override;


			void SetConstantBuffer(const Core::ConstantBufferView cbv, const uint32_t registerIndex) override;


			void UpdateConstantBuffer(Core::ConstantBufferUploadBase* cb) override;


			void UploadData(Core::Buffer* buffer, const void* data, size_t sizeInByte) override;


			void UploadData(Core::ConstantBufferDirectBase* cb) override;


			void SetVertexBuffer(Core::VertexBufferBase* vertexBuffer) override;


			void SetIndexBuffer(Core::IndexBuffer* indexBuffer) override;


			void DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset) override;


			void Draw(uint32_t vertexNum, uint32_t vertexOffset) override;


			void ClearRenderTarget(const Core::RenderTargetView RTV);
			void ClearRenderTarget(const Core::SwapChain* swapChain, const Core::RenderTargetView RTV);
			void ClearDepthStencil(const Core::DepthStencilView DSV);


			void SetViewport(Core::Viewport* viewport, Core::ViewRect* rect);


			void Submit() override;


			void Commit() override;


			void Present() override;


			bool IsRunning() override;


			void WaitToFinish() override;

		protected:


			void _BindResource();

			void _ChangeResourceState(Core::GraphicResource* resource, TE_RESOURCE_STATES newState);


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
				ClearingRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, Core::ClearValue_RenderTarget clearValue) : RTV(rtv), ClearValue(clearValue)
				{}

				D3D12_CPU_DESCRIPTOR_HANDLE RTV;
				Core::ClearValue_RenderTarget ClearValue;
			};


			struct ClearingDepthStencil
			{
				ClearingDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsv, Core::ClearValue_DepthStencil clearValue)
					: DSV(dsv), ClearValue(clearValue)
				{}

				D3D12_CPU_DESCRIPTOR_HANDLE DSV;
				Core::ClearValue_DepthStencil ClearValue;
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

			const DirectX12RootArguments* m_RootArguments;

			//
			// friend class
			//
			friend class DirectX12CommandQueue;
			friend class DirectX12CommandQueue_Direct;
			friend class DirectX12CommandQueue_Copy;

		};


	}
}
