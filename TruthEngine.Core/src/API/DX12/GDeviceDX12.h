#pragma once
#include "CommandQueue.h"
#include "Fence.h"


#define TE_INSTANCE_API_DX12_GDEVICE			TruthEngine::API::DX12::GDeviceDX12::GetPrimaryDevice()
#define TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT TruthEngine::API::DX12::GDeviceDX12::GetPrimaryDevice().GetCommandQueuDirect()
#define TE_INSTANCE_API_DX12_COMMANDQUEUECOPY	TruthEngine::API::DX12::GDeviceDX12::GetPrimaryDevice().GetCommandQueuCopy()


namespace TruthEngine::API::DX12 {

	class GDeviceDX12 {
		friend class CommandQueue;
		friend class CommandList;
		friend class CommandAllocator;
	public:


		GDeviceDX12(UINT adapterIndex)
		{
			Init(adapterIndex);
		}


		inline ID3D12Device8* GetDevice() const noexcept { return m_Device.Get(); }


		

		inline CommandQueue& GetCommandQueuDirect() { return m_CommandQueueDirect; }
		inline CommandQueue& GetCommandQueuCopy() { return m_CommandQueueCopy; }

		inline Fence& GetFence() { return m_Fence; }

		static inline GDeviceDX12& GetPrimaryDevice() { return s_PrimaryDevice; }

	private:
		TE_RESULT Init(UINT adapterIndex);
		TE_RESULT CreateDevice(UINT adapterIndex);
		TE_RESULT InitCommandQueues();
		void InitDescriptorSize()const;

		inline UINT GetDescriptorSizeRTV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); }
		inline UINT GetDescriptorSizeSRV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }
		inline UINT GetDescriptorSizeDSV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV); }
		inline UINT GetDescriptorSizeSampler() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER); }

		TE_RESULT CreateCommandQueue(COMPTR<ID3D12CommandQueue>& cmdQueue, D3D12_COMMAND_LIST_TYPE type) const;

		TE_RESULT CreateCommandList(COMPTR<ID3D12GraphicsCommandList>& cmdList, D3D12_COMMAND_LIST_TYPE type) const;

		TE_RESULT CreateCommandAllocator(COMPTR<ID3D12CommandAllocator>& cmdAlloc, D3D12_COMMAND_LIST_TYPE type) const;
	private:

		Microsoft::WRL::ComPtr<ID3D12Device8> m_Device;

		CommandQueue m_CommandQueueDirect;
		CommandQueue m_CommandQueueCopy;

		Fence m_Fence;

		static GDeviceDX12 s_PrimaryDevice;

	};

}




