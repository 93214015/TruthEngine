#pragma once

namespace TruthEngine::API::DX12
{

#define DESCRIPTORHEAP_GETHANDLES	inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const override { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, m_DescriptorSize); };\
									inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleLast() const override { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentIndex, m_DescriptorSize); }\
									inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index) const override { return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), index, m_DescriptorSize); }\
									inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleLast() const override { return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_CurrentIndex, m_DescriptorSize); }


	class DescriptorHeap
	{
		friend class GDeviceDX12;

	public:
		DescriptorHeap() = default;
		virtual ~DescriptorHeap() = default;

		virtual TE_RESULT Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, UINT nodeMask = 0) = 0;

		inline UINT GetCurrentIndex()const noexcept { return m_CurrentIndex; }

		inline ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_DescriptorHeap.Get(); }

		inline void Release() { m_DescriptorHeap->Release(); }

		virtual inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const = 0;
		virtual inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleLast() const = 0;
		virtual inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index) const = 0;
		virtual inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleLast() const = 0;

		virtual UINT AddDescriptor(ID3D12Resource* resource) = 0;

	protected:
		ID3D12Device* m_Device;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		UINT m_CurrentIndex = 0;
	};


	class DescriptorHeapRTV : public DescriptorHeap {
		friend class GDeviceDX12;
	public:
		virtual ~DescriptorHeapRTV() = default;

		virtual TE_RESULT Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, UINT nodeMask = 0) override;
		virtual UINT AddDescriptor(ID3D12Resource* resource) override;

		DESCRIPTORHEAP_GETHANDLES

		static inline UINT GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		static UINT m_DescriptorSize;
	};


	class DescriptorHeapSRV : public DescriptorHeap {
		friend class GDeviceDX12;
	public:
		virtual ~DescriptorHeapSRV() = default;

		virtual TE_RESULT Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, UINT nodeMask = 0) override;
		virtual UINT AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
		virtual UINT AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
		virtual UINT AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc);

		DESCRIPTORHEAP_GETHANDLES

		static inline UINT GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		UINT AddDescriptor(ID3D12Resource* resource) override { return TE_FAIL; };

	protected:

		static UINT m_DescriptorSize;

	};


	class DescriptorHeapDSV : public DescriptorHeap {
		friend class GDeviceDX12;
	public:
		virtual ~DescriptorHeapDSV() = default;

		virtual TE_RESULT Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, UINT nodeMask = 0) override;
		virtual UINT AddDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc);

		DESCRIPTORHEAP_GETHANDLES

		static inline UINT GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		UINT AddDescriptor(ID3D12Resource* resource) override { return TE_FAIL; }

	protected:

		static UINT m_DescriptorSize;

	};


	class DescriptorHeapSampler : public DescriptorHeap {
		friend class GDeviceDX12;
	public:
		virtual ~DescriptorHeapSampler() = default;

		virtual TE_RESULT Init(ID3D12Device* device, UINT descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, UINT nodeMask = 0) override;
		virtual UINT AddDescriptor(const D3D12_SAMPLER_DESC* samplerDesc);

		DESCRIPTORHEAP_GETHANDLES

		static inline UINT GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		UINT AddDescriptor(ID3D12Resource* resource) override { return TE_FAIL; };

	protected:
		static UINT m_DescriptorSize;

	};

}
