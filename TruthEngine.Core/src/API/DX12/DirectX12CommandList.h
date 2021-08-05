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
				return mD3D12CommandList.Get();
			}


			inline DirectX12CommandAllocator* GetCommandAllocator()
			{
				return &mCommandAllocator;
			}


			void ResetCompute() override;


			void ResetCompute(PipelineCompute* pipeline) override;


			void ResetGraphics() override;


			void ResetGraphics(PipelineGraphics* pipeline) override;


			void SetPipelineGraphics(PipelineGraphics* pipeline) override;


			void SetPipelineCompute(PipelineCompute* pipeline) override;


			//void SetRenderTarget(TextureRenderTarget* RenderTargets[], const uint32_t RenderTargetNum, const TextureDepthStencil* DepthStencil) override;
			void SetRenderTarget(const RenderTargetView RTV) override;
			void SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV) override;


			void SetDepthStencil(const DepthStencilView DSV) override;

			void ResolveMultiSampledTexture(Texture* _SourceTexture, Texture* _DestTexture) override;

			/*void SetShaderResource(const ShaderResourceView srv, const uint32_t registerIndex) override;


			void SetConstantBuffer(const ConstantBufferView cbv, const uint32_t registerIndex) override;*/


			void UpdateConstantBuffer(ConstantBufferUploadBase* cb) override;


			void UploadData(Buffer* buffer, const void* data, size_t sizeInByte) override;


			void SetDirectConstantGraphics(ConstantBufferDirectBase* cb) override;


			void SetDirectConstantCompute(ConstantBufferDirectBase* cb) override;


			void SetDirectViewSRVGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot) override;
			void SetDirectViewCBVGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot) override;
			void SetDirectViewUAVGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot) override;

			void SetDirectViewSRVCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot) override;
			void SetDirectViewCBVCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot) override;
			void SetDirectViewUAVCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot) override;


			void SetVertexBuffer(VertexBufferBase* vertexBuffer) override;


			void SetIndexBuffer(IndexBuffer* indexBuffer) override;


			void DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset) override;


			void Draw(uint32_t vertexNum, uint32_t vertexOffset) override;


			void Dispatch(uint32_t GroupNumX, uint32_t GroupNumY, uint32_t GroupNumZ) override;


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


			void _SetRootArgumentsGraphics(bool _NewShaderClass);
			void _SetRootSignatureGraphics(bool _NewShaderClass);

			void _SetRootArgumentsCompute(bool _NewShaderClass);
			void _SetRootSignatureCompute(bool _NewShaderClass);

			void _ChangeResourceState(GraphicResource* resource, TE_RESOURCE_STATES newState);


			inline void _QueueBarrierTransition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
			{
				m_ResourceBarriers.emplace_back(
					CD3DX12_RESOURCE_BARRIER::Transition(
						resource
						, before
						, after
					)
				);
			}

			inline void _QueueBarrierUAV(ID3D12Resource* resource)
			{
				m_ResourceBarriers.emplace_back(
					CD3DX12_RESOURCE_BARRIER::UAV(
						resource
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

			struct BindPendingDescriptor
			{
				BindPendingDescriptor(ID3D12Resource* _Resource, uint32_t _RootParamterIndex)
					: mResource(_Resource), mRootParamterIndex(_RootParamterIndex)
				{}

				ID3D12Resource* mResource;
				uint32_t mRootParamterIndex;
			};

			struct ResolveResourcePending
			{
				ResolveResourcePending(ID3D12Resource* _SourceResource, ID3D12Resource* _DestinationResource, DXGI_FORMAT _Format)
					: mSourceResource(_SourceResource), mDestinationResource(_DestinationResource), mFormat(_Format)
				{}

				ID3D12Resource* mSourceResource;
				ID3D12Resource* mDestinationResource;
				DXGI_FORMAT mFormat;
			};


			COMPTR<ID3D12GraphicsCommandList> mD3D12CommandList;

			DirectX12CommandAllocator mCommandAllocator;

			DirectX12BufferManager* m_BufferManager;

			DirectX12ShaderManager* m_ShaderManager;

			std::vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;

			/*std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SRVHandles_CB;

			std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SRVHandles_Texture;*/

			std::vector<ClearingRenderTarget> m_QueueClearRT;

			std::vector<ClearingDepthStencil> m_QueueClearDS;

			std::vector<CopyPending_DefaultResource> m_QueueCopyDefaultResource;

			std::vector<ResolveResourcePending> m_QueueResolveResource;

			std::vector<BindPendingDescriptor> mBindPendingGraphicsSRVs;
			std::vector<BindPendingDescriptor> mBindPendingGraphicsUAVs;
			std::vector<BindPendingDescriptor> mBindPendingGraphicsCBVs;
			std::vector<BindPendingDescriptor> mBindPendingComputeSRVs;
			std::vector<BindPendingDescriptor> mBindPendingComputeUAVs;
			std::vector<BindPendingDescriptor> mBindPendingComputeCBVs;

			D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;

			D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandles[8] = { D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE() };

			uint32_t m_RTVHandleNum = 0;

			uint32_t m_DSVHandleNum = 0;

			size_t m_CopyQueueRequiredSize = 0;

			COMPTR<ID3D12Resource> m_IntermediateResource;

			const DirectX12RootArguments* mRootArguments = nullptr;
			ID3D12RootSignature* mRootSignature = nullptr;

			//
			// friend class
			//
			friend class DirectX12CommandQueue;
			friend class DirectX12CommandQueue_Direct;
			friend class DirectX12CommandQueue_Copy;

		};


	}
}
