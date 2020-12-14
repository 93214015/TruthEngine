#include "pch.h"
#include "DX12GraphicDevice.h"

#include "API/IDXGI.h"
#include "API/DX12/DescriptorHeap.h"
#include "API/DX12/DX12SwapChain.h"

#include "Core/Application.h"

namespace TruthEngine::API::DirectX12
{

	TE_RESULT DX12GraphicDevice::Init(UINT adapterIndex)
	{
		TE_INSTANCE_IDXGI.Init();

		CreateDevice(adapterIndex);
		InitCommandQueues();
		InitDescriptorSize();

		m_Fence.Initialize(*this);

		// init singleton object of dx12 swap chain
		TE_INSTANCE_API_DX12_SWAPCHAIN.Init(TE_INSTANCE_APPLICATION->GetClientWidth(), TE_INSTANCE_APPLICATION->GetClientHeight(), static_cast<HWND>(TE_INSTANCE_APPLICATION->GetWindow()->GetNativeWindowHandle()), TE_INSTANCE_APPLICATION->GetFramesInFlightNum());

		return TE_SUCCESSFUL;
	}

	TE_RESULT DX12GraphicDevice::CreateDevice(UINT adapterIndex)
	{

#if defined(TE_DEBUG)
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> debuglayer;
			auto result = D3D12GetDebugInterface(IID_PPV_ARGS(debuglayer.GetAddressOf()));

			TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  Enabling DX12 Debug Layer is failed!");

			debuglayer->EnableDebugLayer();
		}
#endif
		auto result = D3D12CreateDevice(TE_INSTANCE_IDXGI.GetAdapters()[adapterIndex].Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf()));
		TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  Creating DX12 device if failed!");

		return TE_SUCCESSFUL;
	}

	void DX12GraphicDevice::InitDescriptorSize() const
	{
		DescriptorHeapRTV::m_DescriptorSize = GetDescriptorSizeRTV();
		DescriptorHeapSRV::m_DescriptorSize = GetDescriptorSizeSRV();
		DescriptorHeapDSV::m_DescriptorSize = GetDescriptorSizeDSV();
		DescriptorHeapSampler::m_DescriptorSize = GetDescriptorSizeSampler();
	}

	TE_RESULT DX12GraphicDevice::InitCommandQueues()
	{
		auto result = m_CommandQueueCopy.Init(this);
		TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  PrimaryCopyCommandQueue Creation was failed!");
		result = m_CommandQueueDirect.Init(this);
		TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  PrimaryDirectCommandQueue Creation was failed!");


		return TE_SUCCESSFUL;
	}

	TE_RESULT DX12GraphicDevice::CreateCommandQueue(COMPTR<ID3D12CommandQueue>& cmdQueue, D3D12_COMMAND_LIST_TYPE type) const
	{
		D3D12_COMMAND_QUEUE_DESC desc;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = 0;
		desc.Type = type;
		if (TE_SUCCEEDED(m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(cmdQueue.ReleaseAndGetAddressOf()))))
			return TE_SUCCESSFUL;
		else
		{
			TE_LOG_CORE_ERROR("API::DirectX12  CommandQueue creation was failed");
			return TE_FAIL;
		}
	}

	TE_RESULT DX12GraphicDevice::CreateCommandList(COMPTR<ID3D12GraphicsCommandList>& cmdList, D3D12_COMMAND_LIST_TYPE type) const
	{
		D3D12_COMMAND_QUEUE_DESC desc;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = 0;
		desc.Type = type;
		if (m_Device->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(cmdList.ReleaseAndGetAddressOf())) == S_OK)
		{
			return TE_SUCCESSFUL;
		}
		else
		{
			TE_LOG_CORE_ERROR("API::DirectX12  CommandList creation was failed");
			return TE_FAIL;
		}
	}

	TE_RESULT DX12GraphicDevice::CreateCommandAllocator(COMPTR<ID3D12CommandAllocator>& cmdAlloc, D3D12_COMMAND_LIST_TYPE type) const
	{
		if (m_Device->CreateCommandAllocator(type, IID_PPV_ARGS(cmdAlloc.ReleaseAndGetAddressOf())) == S_OK)
			return TE_SUCCESSFUL;
		else
		{
			TE_LOG_CORE_ERROR("API::DirectX12  CommandAllocator creation was failed");
			return TE_FAIL;
		}
	}

	TruthEngine::API::DirectX12::DX12GraphicDevice DX12GraphicDevice::s_PrimaryDevice;

}

#ifdef TE_API_DX12

TE_RESULT TruthEngine::Core::CreateGDevice(uint32_t adapterIndex)
{
	return TruthEngine::API::DirectX12::DX12GraphicDevice::GetPrimaryDeviceDX12().Init(adapterIndex);
}

TruthEngine::Core::GraphicDevice* TruthEngine::Core::GraphicDevice::s_GDevice = &TruthEngine::API::DirectX12::DX12GraphicDevice::GetPrimaryDeviceDX12();

#endif