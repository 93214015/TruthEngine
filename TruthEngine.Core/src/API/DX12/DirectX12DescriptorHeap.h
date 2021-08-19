#pragma once

namespace TruthEngine::API::DirectX12
{

	class DirectX12GraphicDevice;

#define DESCRIPTORHEAP_GETHANDLES	inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index) const override { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, m_DescriptorSize); };\
									inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleLast() const override { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentIndex, m_DescriptorSize); }\
									inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index) const override { return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), index, m_DescriptorSize); }\
									inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleLast() const override { return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_CurrentIndex, m_DescriptorSize); }


	class DirectX12DescriptorHeap
	{
		friend class DirectX12GraphicDevice;

	public:
		DirectX12DescriptorHeap(uint32_t reserveDescriptors = 0);
		virtual ~DirectX12DescriptorHeap() = default;

		virtual TE_RESULT Init(DirectX12GraphicDevice& gDeviceDX12, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t nodeMask = 0) = 0;

		inline uint32_t GetCurrentIndex()const noexcept { return m_CurrentIndex; }

		inline ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_DescriptorHeap.Get(); }

		inline void Release() { m_DescriptorHeap->Release(); }

		virtual inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index) const = 0;
		virtual inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleLast() const = 0;
		virtual inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index) const = 0;
		virtual inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleLast() const = 0;

		//virtual uint32_t AddDescriptor(ID3D12Resource* resource, const void* _D3D12ViewDesc = nullptr) = 0;
		//virtual void ReplaceDescriptor(ID3D12Resource* resource, uint32_t index, const void* _D3D12ViewDesc = nullptr) = 0;

	protected:
		DirectX12GraphicDevice* m_Device;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		uint32_t m_CurrentIndex = 0;
	};


	class DescriptorHeapRTV final : public DirectX12DescriptorHeap {
		friend class DirectX12GraphicDevice;
	public:
		using DirectX12DescriptorHeap::DirectX12DescriptorHeap;
		virtual ~DescriptorHeapRTV() = default;

		virtual TE_RESULT Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t nodeMask = 0) override;

		uint32_t AddDescriptor(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* _D3D12RenderTargetViewDesc = nullptr);
		void ReplaceDescriptor(ID3D12Resource* resource, uint32_t index, const D3D12_RENDER_TARGET_VIEW_DESC* _D3D12RenderTargetViewDesc = nullptr);

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		static uint32_t m_DescriptorSize;
	};


	class DescriptorHeapSRV final : public DirectX12DescriptorHeap {
		friend class DirectX12GraphicDevice;
	public:
		using DirectX12DescriptorHeap::DirectX12DescriptorHeap;
		virtual ~DescriptorHeapSRV() = default;

		virtual TE_RESULT Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, uint32_t nodeMask = 0) override;
		uint32_t AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
		void ReplaceDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, uint32_t index);
		uint32_t AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
		void ReplaceDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc, uint32_t index);
		uint32_t AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc);
		void ReplaceDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, uint32_t index);

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:

	protected:

		static uint32_t m_DescriptorSize;

	};


	class DescriptorHeapDSV final : public DirectX12DescriptorHeap {
		friend class DirectX12GraphicDevice;
	public:
		using DirectX12DescriptorHeap::DirectX12DescriptorHeap;
		virtual ~DescriptorHeapDSV() = default;

		virtual TE_RESULT Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t nodeMask = 0) override;
		virtual uint32_t AddDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* _D3D12DepthStencilViewDesc);
		virtual void ReplaceDescriptor(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* _D3D12DepthStencilViewDesc, uint32_t index);

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:
		

	protected:

		static uint32_t m_DescriptorSize;

	};


	class DescriptorHeapSampler : public DirectX12DescriptorHeap {
		friend class DirectX12GraphicDevice;
	public:
		using DirectX12DescriptorHeap::DirectX12DescriptorHeap;
		virtual ~DescriptorHeapSampler() = default;

		virtual TE_RESULT Init(DirectX12GraphicDevice& device, uint32_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, uint32_t nodeMask = 0) override;
		virtual uint32_t AddDescriptor(const D3D12_SAMPLER_DESC* samplerDesc);
		virtual void ReplaceDescriptor(const D3D12_SAMPLER_DESC* samplerDesc, uint32_t index);

		DESCRIPTORHEAP_GETHANDLES

		static inline uint32_t GetDescriptorSize() { return m_DescriptorSize; }

	protected:

	protected:
		static uint32_t m_DescriptorSize;

	};

}
