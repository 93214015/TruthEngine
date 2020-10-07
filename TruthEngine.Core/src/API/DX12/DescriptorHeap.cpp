#include "pch.h"
#include "DescriptorHeap.h"

namespace TruthEngine::API::DX12
{

	TE_RESULT DescriptorHeapRTV::Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_NONE*/, UINT nodeMask /*= 0*/)
	{
		m_Device = device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESFUL;
	}

	UINT DescriptorHeapRTV::AddDescriptor(ID3D12Resource* resource)
	{
		m_Device->CreateRenderTargetView(resource, nullptr, GetCPUHandleLast());
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	TE_RESULT DescriptorHeapSRV::Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE*/, UINT nodeMask /*= 0*/)
	{
		m_Device = device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESFUL;
	}

	UINT DescriptorHeapSRV::AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{
		m_Device->CreateShaderResourceView(resource, srvDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	UINT DescriptorHeapSRV::AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
	{
		m_Device->CreateConstantBufferView(cbvDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	UINT DescriptorHeapSRV::AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
	{
		m_Device->CreateUnorderedAccessView(resource, counterResource, uavDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	TE_RESULT DescriptorHeapDSV::Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_NONE*/, UINT nodeMask /*= 0*/)
	{
		m_Device = device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESFUL;
	}

	UINT DescriptorHeapDSV::AddDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc)
	{
		m_Device->CreateDepthStencilView(resource, dsvDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	TE_RESULT DescriptorHeapSampler::Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE*/, UINT nodeMask /*= 0*/)
	{
		m_Device = device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESFUL;
	}

	UINT DescriptorHeapSampler::AddDescriptor(const D3D12_SAMPLER_DESC* samplerDesc)
	{
		m_Device->CreateSampler(samplerDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

}