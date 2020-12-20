#include "pch.h"
#include "DirectX12DescriptorHeap.h"
#include "DirectX12GraphicDevice.h"

namespace TruthEngine::API::DirectX12
{

	uint32_t DescriptorHeapRTV::m_DescriptorSize = 0;
	uint32_t DescriptorHeapSRV::m_DescriptorSize = 0;
	uint32_t DescriptorHeapDSV::m_DescriptorSize = 0;
	uint32_t DescriptorHeapSampler::m_DescriptorSize = 0;


	TE_RESULT DescriptorHeapRTV::Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_NONE*/, uint32_t nodeMask /*= 0*/)
	{
		m_Device = &device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESSFUL;
	}

	uint32_t DescriptorHeapRTV::AddDescriptor(ID3D12Resource* resource)
	{
		m_Device->GetDevice()->CreateRenderTargetView(resource, nullptr, GetCPUHandleLast());
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	void DescriptorHeapRTV::ReplaceDescriptor(ID3D12Resource* resource, uint32_t index)
	{
		m_Device->GetDevice()->CreateRenderTargetView(resource, nullptr, GetCPUHandle(index));
	}

	TE_RESULT DescriptorHeapSRV::Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE*/, uint32_t nodeMask /*= 0*/)
	{
		m_Device = &device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESSFUL;
	}

	uint32_t DescriptorHeapSRV::AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{
		m_Device->GetDevice()->CreateShaderResourceView(resource, srvDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	void DescriptorHeapSRV::ReplaceDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, uint32_t index)
	{
		m_Device->GetDevice()->CreateShaderResourceView(resource, srvDesc, GetCPUHandle(index));
	}

	uint32_t DescriptorHeapSRV::AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
	{
		m_Device->GetDevice()->CreateConstantBufferView(cbvDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	void DescriptorHeapSRV::ReplaceDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc, uint32_t index)
	{
		m_Device->GetDevice()->CreateConstantBufferView(cbvDesc, GetCPUHandle(index));
	}

	uint32_t DescriptorHeapSRV::AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
	{
		m_Device->GetDevice()->CreateUnorderedAccessView(resource, counterResource, uavDesc, GetCPUHandle(m_CurrentIndex));
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	void DescriptorHeapSRV::ReplaceDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, uint32_t index)
	{
		m_Device->GetDevice()->CreateUnorderedAccessView(resource, counterResource, uavDesc, GetCPUHandle(index));
	}

	TE_RESULT DescriptorHeapDSV::Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_NONE*/, uint32_t nodeMask /*= 0*/)
	{
		m_Device = &device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESSFUL;
	}

	uint32_t DescriptorHeapDSV::AddDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc)
	{
		m_Device->GetDevice()->CreateDepthStencilView(resource, dsvDesc, GetCPUHandleLast());
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	void DescriptorHeapDSV::ReplaceDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc, uint32_t index)
	{
		m_Device->GetDevice()->CreateDepthStencilView(resource, dsvDesc, GetCPUHandle(index));
	}

	TE_RESULT DescriptorHeapSampler::Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE*/, uint32_t nodeMask /*= 0*/)
	{
		m_Device = &device;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors = descriptorNum;
		desc.Flags = flags;
		desc.NodeMask = nodeMask;

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.ReleaseAndGetAddressOf()));

		return TE_SUCCESSFUL;
	}

	uint32_t DescriptorHeapSampler::AddDescriptor(const D3D12_SAMPLER_DESC* samplerDesc)
	{
		m_Device->GetDevice()->CreateSampler(samplerDesc, GetCPUHandleLast());
		auto insertedIndex = m_CurrentIndex;
		m_CurrentIndex++;
		return insertedIndex;
	}

	void DescriptorHeapSampler::ReplaceDescriptor(const D3D12_SAMPLER_DESC* samplerDesc, uint32_t index)
	{
		m_Device->GetDevice()->CreateSampler(samplerDesc, GetCPUHandle(index));
	}

}