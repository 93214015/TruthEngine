#pragma once
#include "DirectX12CommandQueue.h"
#include "DirectX12Fence.h"
#include "Core/Renderer/GraphicDevice.h"


#define TE_INSTANCE_API_DX12_GRAPHICDEVICE			TruthEngine::API::DirectX12::DirectX12GraphicDevice::GetPrimaryDeviceDX12()
#define TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT		TruthEngine::API::DirectX12::DirectX12GraphicDevice::GetPrimaryDeviceDX12().GetCommandQueuDirect()
#define TE_INSTANCE_API_DX12_COMMANDQUEUECOPY		TruthEngine::API::DirectX12::DirectX12GraphicDevice::GetPrimaryDeviceDX12().GetCommandQueuCopy()

namespace TruthEngine::Core {

	class GraphicResource;

}

namespace TruthEngine::API::DirectX12 {


	class DirectX12GraphicDevice : public TruthEngine::Core::GraphicDevice {
		
	public:
		TE_RESULT Init(UINT adapterIndex) override;

		inline ID3D12Device8* GetDevice() const noexcept { return m_Device.Get(); }

		inline ID3D12Device8* operator->() { return m_Device.Get(); }
		
		void WaitForGPU() override;

		inline DirectX12CommandQueue_Direct* GetCommandQueuDirect() { return &m_CommandQueueDirect; }
		inline DirectX12CommandQueue_Copy* GetCommandQueuCopy() { return &m_CommandQueueCopy; }
		inline DirectX12Fence& GetFence() { return m_Fence; }
		inline const D3D12_FEATURE_DATA_ROOT_SIGNATURE& GetFeature_RootSignature() { return m_FeatureData_RootSignature; }

		static inline DirectX12GraphicDevice& GetPrimaryDeviceDX12() { return s_PrimaryDevice; }

	private:
		TE_RESULT CreateDevice(UINT adapterIndex);
		TE_RESULT InitCommandQueues();
		void InitDescriptorSize()const;
		void CheckFeatures();

		inline UINT GetDescriptorSizeRTV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); }
		inline UINT GetDescriptorSizeSRV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }
		inline UINT GetDescriptorSizeDSV() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV); }
		inline UINT GetDescriptorSizeSampler() const noexcept { return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER); }


		TE_RESULT CreateCommandQueue(COMPTR<ID3D12CommandQueue>& cmdQueue, D3D12_COMMAND_LIST_TYPE type) const;

		TE_RESULT CreateCommandList(COMPTR<ID3D12GraphicsCommandList>& cmdList, D3D12_COMMAND_LIST_TYPE type) const;

		TE_RESULT CreateCommandAllocator(COMPTR<ID3D12CommandAllocator>& cmdAlloc, D3D12_COMMAND_LIST_TYPE type) const;
	private:

		Microsoft::WRL::ComPtr<ID3D12Device8> m_Device;

		DirectX12CommandQueue_Direct m_CommandQueueDirect;
		DirectX12CommandQueue_Copy m_CommandQueueCopy;

		DirectX12Fence m_Fence;

		HANDLE m_EventGPUWorkFinished_DirectQueue;
		HANDLE m_EventGPUWorkFinished_CopyQueue;


		D3D12_FEATURE_DATA_ROOT_SIGNATURE m_FeatureData_RootSignature = {};

		static DirectX12GraphicDevice s_PrimaryDevice;

		/// Friend Classes
		friend class DirectX12CommandQueue;
		friend class DirectX12CommandList;
		friend class DirectX12CommandAllocator;

	};

}




