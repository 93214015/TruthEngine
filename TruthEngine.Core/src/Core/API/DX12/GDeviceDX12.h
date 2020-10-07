#pragma once


namespace TruthEngine::Core::API::DX12 {

	class GDeviceDX12 {

	public:


		GDeviceDX12(UINT adapterIndex)
		{
			Init(adapterIndex);
		}


		inline ID3D12Device* GetDevice() const noexcept { return m_Device.Get(); }


		inline void CreateCommandQueue(COMPTR<ID3D12CommandQueue>& cmdQueue, D3D12_COMMAND_LIST_TYPE type){
			D3D12_COMMAND_QUEUE_DESC desc;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;
			desc.Priority = 0;
			desc.Type = type;
			m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(cmdQueue.ReleaseAndGetAddressOf()));
		}

		inline void CreateCommandList(COMPTR<ID3D12CommandList>& cmdList, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* cmdAllocator, ID3D12PipelineState* pipelineState) {
			D3D12_COMMAND_QUEUE_DESC desc;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;
			desc.Priority = 0;
			desc.Type = type;
			m_Device->CreateCommandList(0, type, cmdAllocator, pipelineState, IID_PPV_ARGS(cmdList.ReleaseAndGetAddressOf()));
		}

		inline void CreateCommandAllocator(COMPTR<ID3D12CommandAllocator>& cmdAlloc, D3D12_COMMAND_LIST_TYPE type)
		{
			m_Device->CreateCommandAllocator(type, IID_PPV_ARGS(cmdAlloc.ReleaseAndGetAddressOf()));
		}

		inline ID3D12CommandQueue* GetCommandQueuDirect() { return m_CommandQueueDirect.Get(); }
		inline ID3D12CommandQueue* GetCommandQueuCopy() { return m_CommandQueueCopy.Get(); }

		static inline GDeviceDX12& GetPrimaryDevice() { return s_PrimaryDevice; }

	private:
		void Init(UINT adapterIndex);
		void CreateDevice(UINT adapterIndex);
		void CreateCommandQueues();
		void InitDescriptorSize()const;

		inline UINT GetDescriptorSizeRTV() const noexcept{ return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); }
		inline UINT GetDescriptorSizeSRV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }
		inline UINT GetDescriptorSizeDSV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV); }
		inline UINT GetDescriptorSizeSampler() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER); }


	private:

		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueueDirect;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueueCopy;

		static GDeviceDX12 s_PrimaryDevice;

	};

}

#define TE_INSTANCE_GDEVICEDX12 TruthEngine::Core::API::DX12::GDeviceDX12::GetPrimaryDevice()
#define TE_INSTANCE_COMMANDQUEUEDIRECT TruthEngine::Core::API::DX12::GDeviceDX12::GetPrimaryDevice().GetCommandQueuDirect()
#define TE_INSTANCE_COMMANDQUEUECOPY TruthEngine::Core::API::DX12::GDeviceDX12::GetPrimaryDevice().GetCommandQueuCopy()
