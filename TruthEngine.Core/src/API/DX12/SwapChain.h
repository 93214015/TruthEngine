#pragma once
#include "API/DX12/GDeviceDX12.h"
#include "API/DX12/DescriptorHeap.h"


namespace TruthEngine::API::DX12 {

	class SwapChain
	{

	public:
		SwapChain();

		TE_RESULT Init(UINT clientWidth, UINT clientHeight, HWND* outputHWND);

		TE_RESULT InitRTVs(DescriptorHeapRTV* descHeap);

		inline void ChangeResourceState(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) { barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GetBackBufferResource(), stateBefore, stateAfter)); }

		inline UINT GetCurrentFrameIndex() const noexcept { return m_BackBufferIndex; };

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferRTV() const { return m_DescHeapRTV->GetCPUHandle(static_cast<INT>(m_BackBufferIndex + m_BackBufferDescOffset)); }

		inline ID3D12Resource* GetBackBufferResource() const { return m_BackBuffers[m_BackBufferIndex].Get(); };

		void Present();


	protected:

		void CreateSwapChain(HWND* outputHWND);

		void CreateSwapChainRTVs();

		void CheckDeviceFeatures();

	protected:

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain = nullptr;

		DescriptorHeapRTV* m_DescHeapRTV = nullptr;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_BackBuffers{};

		DXGI_FORMAT m_BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS m_MSAAQualityLevels{};

		UINT m_BackBufferIndex = 0;
		UINT m_BackBufferDescOffset = 0;
		UINT m_BackBufferNum = 2;

		bool m_UseMSAA4X = false;

	};

}