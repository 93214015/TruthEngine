#pragma once
#include "CommandQueue.h"
#include "Fence.h"
#include "Core/Renderer/GraphicDevice.h"


#define TE_INSTANCE_API_DX12_GRAPHICDEVICE			TruthEngine::API::DirectX12::DX12GraphicDevice::GetPrimaryDeviceDX12()
#define TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT		TruthEngine::API::DirectX12::DX12GraphicDevice::GetPrimaryDeviceDX12().GetCommandQueuDirect()
#define TE_INSTANCE_API_DX12_COMMANDQUEUECOPY		TruthEngine::API::DirectX12::DX12GraphicDevice::GetPrimaryDeviceDX12().GetCommandQueuCopy()

namespace TruthEngine::Core {

	class GraphicResource;

}

namespace TruthEngine::API::DirectX12 {


	class DX12GraphicDevice : public TruthEngine::Core::GraphicDevice {
		
	public:
		TE_RESULT Init(UINT adapterIndex) override;

		inline ID3D12Device8* GetDevice() const noexcept { return m_Device.Get(); }

		inline ID3D12Device8* operator->() { return m_Device.Get(); }
		

		inline CommandQueue_Direct* GetCommandQueuDirect() { return &m_CommandQueueDirect; }
		inline CommandQueue_Copy* GetCommandQueuCopy() { return &m_CommandQueueCopy; }
		inline Fence& GetFence() { return m_Fence; }

		static inline DX12GraphicDevice& GetPrimaryDeviceDX12() { return s_PrimaryDevice; }

	private:
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

		CommandQueue_Direct m_CommandQueueDirect;
		CommandQueue_Copy m_CommandQueueCopy;

		Fence m_Fence;

		static DX12GraphicDevice s_PrimaryDevice;

		/// Friend Classes
		friend class CommandQueue;
		friend class DX12CommandList;
		friend class DX12CommandAllocator;

	};

}




