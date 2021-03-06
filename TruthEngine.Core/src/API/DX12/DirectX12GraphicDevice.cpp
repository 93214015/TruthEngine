#include "pch.h"
#include "DirectX12GraphicDevice.h"

#include "API/IDXGI.h"
#include "API/DX12/DirectX12DescriptorHeap.h"
#include "API/DX12/DirectX12SwapChain.h"

#include "Core/Application.h"

namespace TruthEngine::API::DirectX12
{

	TE_RESULT DirectX12GraphicDevice::Init(UINT adapterIndex)
	{
		TE_INSTANCE_IDXGI.Init();

		CreateDevice(adapterIndex);
		InitCommandQueues();
		InitDescriptorSize();

		m_Fence.Initialize(*this);

		m_EventGPUWorkFinished_DirectQueue = CreateEvent(NULL, false, true, L"");
		m_EventGPUWorkFinished_CopyQueue = CreateEvent(NULL, false, true, L"");

		return TE_SUCCESSFUL;
	}

	void DirectX12GraphicDevice::WaitForGPU()
	{
		m_Fence.SetFenceAndEvent(m_CommandQueueDirect.GetNativeObject(), m_EventGPUWorkFinished_DirectQueue);

		WaitForSingleObject(m_EventGPUWorkFinished_DirectQueue, INFINITE);
	}

	TE_RESULT DirectX12GraphicDevice::CreateDevice(UINT adapterIndex)
	{

#if defined(TE_DEBUG)
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> debuglayer;
			Microsoft::WRL::ComPtr<ID3D12Debug1> debuglayer1;
			auto result = D3D12GetDebugInterface(IID_PPV_ARGS(debuglayer.GetAddressOf()));
			/*debuglayer->QueryInterface(IID_PPV_ARGS(&debuglayer1));
			debuglayer1->SetEnableGPUBasedValidation(true);*/

			TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  Enabling DX12 Debug Layer is failed!");

			debuglayer->EnableDebugLayer();
		}
#endif
		auto result = D3D12CreateDevice(TE_INSTANCE_IDXGI.GetAdapters()[adapterIndex].Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf()));
		TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  Creating DX12 device if failed!");


#ifdef _PROFILING
		m_Device->SetStablePowerState(true);
#endif // _PROFILING

		CheckFeatures();

		return TE_SUCCESSFUL;
	}

	void DirectX12GraphicDevice::InitDescriptorSize() const
	{
		DescriptorHeapRTV::m_DescriptorSize = GetDescriptorSizeRTV();
		DescriptorHeapSRV::m_DescriptorSize = GetDescriptorSizeSRV();
		DescriptorHeapDSV::m_DescriptorSize = GetDescriptorSizeDSV();
		DescriptorHeapSampler::m_DescriptorSize = GetDescriptorSizeSampler();
	}

	TE_RESULT DirectX12GraphicDevice::InitCommandQueues()
	{
		auto result = m_CommandQueueCopy.Init(this);
		TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  PrimaryCopyCommandQueue Creation was failed!");
		result = m_CommandQueueDirect.Init(this);
		TE_ASSERT_CORE(TE_SUCCEEDED(result), "API::DirectX12  PrimaryDirectCommandQueue Creation was failed!");

		//HRESULT _hr = m_CommandQueueDirect.GetNativeObject()->QueryInterface(IID_PPV_ARGS(m_DebugCommandQueue.ReleaseAndGetAddressOf()));

		return TE_SUCCESSFUL;
	}

	TE_RESULT DirectX12GraphicDevice::CreateCommandQueue(COMPTR<ID3D12CommandQueue>& cmdQueue, D3D12_COMMAND_LIST_TYPE type) const
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

	TE_RESULT DirectX12GraphicDevice::CreateCommandList(COMPTR<ID3D12GraphicsCommandList>& cmdList, D3D12_COMMAND_LIST_TYPE type) const
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

	TE_RESULT DirectX12GraphicDevice::CreateCommandAllocator(COMPTR<ID3D12CommandAllocator>& cmdAlloc, D3D12_COMMAND_LIST_TYPE type) const
	{
		if (m_Device->CreateCommandAllocator(type, IID_PPV_ARGS(cmdAlloc.ReleaseAndGetAddressOf())) == S_OK)
			return TE_SUCCESSFUL;
		else
		{
			TE_LOG_CORE_ERROR("API::DirectX12  CommandAllocator creation was failed");
			return TE_FAIL;
		}
	}

	void DirectX12GraphicDevice::CheckFeatures()
	{

		// Check Feature Support :  Root Signature Version		
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &m_FeatureData_RootSignature, sizeof(m_FeatureData_RootSignature))))
		{
			m_FeatureData_RootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}


		//Check Feature Support : MultiSampling
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS _FeatureData_MSAA = {};
		_FeatureData_MSAA.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_FeatureData_MSAA.SampleCount = 4;
		_FeatureData_MSAA.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

		m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &_FeatureData_MSAA, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
		m_MSAA_QualityLevels_4X = _FeatureData_MSAA.NumQualityLevels;

		_FeatureData_MSAA.SampleCount = 8;

		m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &_FeatureData_MSAA, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
		m_MSAA_QualityLevels_8X = _FeatureData_MSAA.NumQualityLevels;

	}

	TruthEngine::API::DirectX12::DirectX12GraphicDevice DirectX12GraphicDevice::s_PrimaryDevice;

}

#ifdef TE_API_DX12

TE_RESULT TruthEngine::CreateGDevice(uint32_t adapterIndex)
{
	return TruthEngine::API::DirectX12::DirectX12GraphicDevice::GetPrimaryDeviceDX12().Init(adapterIndex);
}

TruthEngine::GraphicDevice* TruthEngine::GraphicDevice::s_GDevice = &TruthEngine::API::DirectX12::DirectX12GraphicDevice::GetPrimaryDeviceDX12();

#endif