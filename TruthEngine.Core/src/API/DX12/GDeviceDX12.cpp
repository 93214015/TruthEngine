#include "pch.h"
#include "GDeviceDX12.h"
#include "API/IDXGI.h"
#include "API/DX12/DescriptorHeap.h"

namespace TruthEngine::API::DX12
{

	void GDeviceDX12::Init(UINT adapterIndex)
	{
		CreateDevice(adapterIndex);
		InitDescriptorSize();
	}

	void GDeviceDX12::CreateDevice(UINT adapterIndex)
	{

#if defined(TE_DEBUG)
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> debuglayer;
			auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debuglayer.GetAddressOf()));
			debuglayer->EnableDebugLayer();
		}
#endif

		D3D12CreateDevice(TE_INSTANCE_IDXGI.GetAdapters()[adapterIndex].Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf()));
	}

	void GDeviceDX12::InitDescriptorSize() const
	{
		DescriptorHeapRTV::m_DescriptorSize = GetDescriptorSizeRTV();
		DescriptorHeapSRV::m_DescriptorSize = GetDescriptorSizeSRV();
		DescriptorHeapDSV::m_DescriptorSize = GetDescriptorSizeDSV();
		DescriptorHeapSampler::m_DescriptorSize = GetDescriptorSizeSampler();
	}

	void GDeviceDX12::CreateCommandQueues()
	{
		D3D12_COMMAND_QUEUE_DESC desc;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = 0;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CommandQueueDirect.ReleaseAndGetAddressOf()));

		desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

		m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CommandQueueCopy.ReleaseAndGetAddressOf()));
	}

	TruthEngine::API::DX12::GDeviceDX12 GDeviceDX12::s_PrimaryDevice(0);

}