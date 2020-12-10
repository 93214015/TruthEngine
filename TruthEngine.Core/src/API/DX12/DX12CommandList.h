#pragma once
#include "Core/Renderer/CommandList.h"
#include "DX12CommandAllocator.h"


namespace TruthEngine
{
	
	namespace API::DirectX12 
	{

		class DX12BufferManager;
		class DX12ShaderManager;
		class DX12PiplineManager;


		class DX12CommandList : public Core::CommandList
		{

		public:
			DX12CommandList(Core::GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, Core::BufferManager* bufferManager, Core::ShaderManager* shaderManager);


			void Init(Core::GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, Core::BufferManager* bufferManager, Core::ShaderManager* shaderManager);


			void Release() override;


			inline ID3D12GraphicsCommandList* GetNativeObject() const
			{
				return m_D3D12CommandList.Get();
			}

			inline DX12CommandAllocator* GetCommandAllocator()
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


			void ChangeResourceState(Core::GraphicResource* resource, TE_RESOURCE_STATES newState) override;


			void SetVertexBuffer(Core::VertexBufferBase* vertexBuffer) override;


			void SetIndexBuffer(Core::IndexBuffer* indexBuffer) override;


			void DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset) override;


			void Draw(uint32_t vertexNum, uint32_t vertexOffset) override;

			void ClearRenderTarget(const Core::RenderTargetView RTV);
			void ClearRenderTarget(const Core::SwapChain* swapChain, const Core::RenderTargetView RTV);
			void ClearDepthStencil(const Core::DepthStencilView DSV);

			void Submit() override;
			void Commit() override;

			void Present() override;
		protected:


			inline void _ChangeResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
			{
				m_ResourceBarriers.emplace_back(
					CD3DX12_RESOURCE_BARRIER::Transition(
						resource
						, before
						, after
					)
				);
			}


		protected:

			struct ClearingRenderTarget
			{
				ClearingRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, Core::TextureRenderTarget::ClearValue clearValue) : RTV(rtv), ClearValue(clearValue)
				{}

				D3D12_CPU_DESCRIPTOR_HANDLE RTV;
				Core::TextureRenderTarget::ClearValue ClearValue;
			};

			struct ClearingDepthStencil
			{
				ClearingDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsv, Core::TextureDepthStencil::ClearValue clearValue)
					: DSV(dsv), ClearValue(clearValue)
				{}

				D3D12_CPU_DESCRIPTOR_HANDLE DSV;
				Core::TextureDepthStencil::ClearValue ClearValue;
			};

			COMPTR<ID3D12GraphicsCommandList> m_D3D12CommandList;
			DX12CommandAllocator m_CommandAllocator;

			DX12BufferManager* m_BufferManager;
			DX12ShaderManager* m_ShaderManager;

			std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SRVHandles_CB;
			std::map<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_SRVHandles_Texture;

			D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;
			D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandles[8] = { D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_CPU_DESCRIPTOR_HANDLE() , D3D12_CPU_DESCRIPTOR_HANDLE() };
			uint32_t m_RTVHandleNum = 0;
			uint32_t m_DSVHandleNum = 0;

			std::vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;

			std::vector<ClearingRenderTarget> m_QueueClearRT;
			std::vector<ClearingDepthStencil> m_QueueClearDS;


			//
			// friend class
			//
			friend class CommandQueue;

		};

		/*class DX12CommandList
		{

		public:
			DX12CommandList() = default;

			void Init(uint32_t FrameInflyNum, D3D12_COMMAND_LIST_TYPE type, DX12GraphicDevice& gDevice);

			inline ID3D12GraphicsCommandList* Get() { return m_CommandList.Get(); }

			ID3D12GraphicsCommandList* operator ->() { return m_CommandList.Get(); }

			inline DX12CommandAllocator* GetActiveCommandAllocator() const { return m_ActiveCommandAllocator; }

			void Reset(uint32_t frameIndex, ID3D12PipelineState* pipelineState);

			inline void Release() {
				for (auto& cmdAlloc : m_CommandAllocators)
				{
					cmdAlloc.Release();
				}
				m_CommandList.Reset();
			}


		private:


		private:

			COMPTR<ID3D12GraphicsCommandList> m_CommandList;
			std::vector<DX12CommandAllocator> m_CommandAllocators;

			DX12CommandAllocator* m_ActiveCommandAllocator = nullptr;

			/// 
			/// Friend Classes
			/// 
			friend class DX12GraphicDevice;
		};*/

	}
}
