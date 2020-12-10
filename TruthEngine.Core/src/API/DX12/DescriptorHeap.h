#pragma once

namespace TruthEngine::API::DirectX12
{

	class DX12GraphicDevice;

#define DESCRIPTORHEAP_GETHANDLES	inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index) const override { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, m_DescriptorSize); };\
									inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleLast() const override { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentIndex, m_DescriptorSize); }\
									inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index) const override { return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), index, m_DescriptorSize); }\
									inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleLast() const override { return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_CurrentIndex, m_DescriptorSize); }


	class DescriptorHeap
	{
		friend class DX12GraphicDevice;

	public:
		DescriptorHeap() = default;
		virtual ~DescriptorHeap() = default;

		virtual TE_RESULT Init(DX12GraphicDevice& gDeviceDX12, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t nodeMask = 0) = 0;

		inline uint32_t GetCurrentIndex()const noexcept { return m_CurrentIndex; }

		inline ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_DescriptorHeap.Get(); }

		inline void Release() { m_DescriptorHeap->Release(); }

		virtual inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index) const = 0;
		virtual inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleLast() const = 0;
		virtual inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index) const = 0;
		virtual inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleLast() const = 0;

		virtual uint32_t AddDescriptor(ID3D12Resource* resource) = 0;
		virtual void ReplaceDescriptor(ID3D12Resource* resource, uint32_t index) = 0;

	protected:
		DX12GraphicDevice* m_Device;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		uint32_t m_CurrentIndex = 0;
	};


	class DescriptorHeapRTV : public DescriptorHeap {
		friend class DX12GraphicDevice;
	public:
		virtual ~DescriptorHeapRTV() = default;

		virtual TE_RESULT Init(DX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t nodeMask = 0) override;
		virtual uint32_t AddDescriptor(ID3D12Resource* resource) override;
		virtual void ReplaceDescriptor(ID3D12Resource* resource, uint32_t index) override;

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		static uint32_t m_DescriptorSize;
	};


	class DescriptorHeapSRV : public DescriptorHeap {
		friend class DX12GraphicDevice;
	public:
		virtual ~DescriptorHeapSRV() = default;

		virtual TE_RESULT Init(DX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, uint32_t nodeMask = 0) override;
		virtual uint32_t AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
		virtual void ReplaceDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, uint32_t index);
		virtual uint32_t AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
		virtual void ReplaceDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc, uint32_t index);
		virtual uint32_t AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc);
		virtual void ReplaceDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, uint32_t index);

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		uint32_t AddDescriptor(ID3D12Resource* resource) override { return TE_FAIL; };
		void ReplaceDescriptor(ID3D12Resource* resource, uint32_t index) override { };

	protected:

		static uint32_t m_DescriptorSize;

	};


	class DescriptorHeapDSV : public DescriptorHeap {
		friend class DX12GraphicDevice;
	public:
		virtual ~DescriptorHeapDSV() = default;

		virtual TE_RESULT Init(DX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t nodeMask = 0) override;
		virtual uint32_t AddDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc);
		virtual void ReplaceDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc, uint32_t index);

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		uint32_t AddDescriptor(ID3D12Resource* resource) override { return TE_FAIL; }
		virtual void ReplaceDescriptor(ID3D12Resource* resource, uint32_t index) override {};

	protected:

		static uint32_t m_DescriptorSize;

	};


	class DescriptorHeapSampler : public DescriptorHeap {
		friend class DX12GraphicDevice;
	public:
		virtual ~DescriptorHeapSampler() = default;

		virtual TE_RESULT Init(DX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, uint32_t nodeMask = 0) override;
		virtual uint32_t AddDescriptor(const D3D12_SAMPLER_DESC* samplerDesc);
		virtual void ReplaceDescriptor(const D3D12_SAMPLER_DESC* samplerDesc, uint32_t index);

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		uint32_t AddDescriptor(ID3D12Resource* resource) override { return TE_FAIL; };
		void ReplaceDescriptor(ID3D12Resource* resource, uint32_t index) override { };

	protected:
		static uint32_t m_DescriptorSize;

	};

}
