#pragma once
#include "API/DX12/DX12GraphicDevice.h"
#include "API/DX12/CommandList.h"
#include "API/DX12/DescriptorHeap.h"
#include "Core/Application.h"
#include "Core/Renderer/SwapChain.h"



namespace TruthEngine::API::DX12 {

	class DX12SwapChain : public TruthEngine::Core::SwapChain
	{

	public:
		static inline DX12SwapChain& Get() { return s_SwapChain; }


		TE_RESULT Init(UINT clientWidth, UINT clientHeight, HWND outputHWND, UINT backBufferNum = 2);

		TE_RESULT Resize(UINT width, UINT height, UINT backBufferNum);

		uint32_t InitRTVs(DescriptorHeapRTV* descHeap);

		inline void ChangeResourceState(D3D12_RESOURCE_BARRIER& barrier, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) { barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetBackBufferResource(), stateBefore, stateAfter); }

		inline UINT GetCurrentFrameIndex() const noexcept { return TE_INSTANCE_APPLICATION.GetCurrentFrameIndex(); };

		inline ID3D12Resource* GetBackBufferResource() const { return m_BackBuffers[GetCurrentFrameIndex()].Get(); };

		inline UINT GetBackBufferNum()const noexcept { return m_BackBufferNum; }

		void Present() override;


	protected:
		DX12SwapChain();

		void CreateSwapChain(HWND outputHWND);

		uint32_t CreateSwapChainRTVs(DescriptorHeapRTV* descHeap);

		void CheckDeviceFeatures();

	protected:

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain = nullptr;

		CommandList m_CommandList;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_BackBuffers{};

		DXGI_FORMAT m_BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS m_MSAAQualityLevels{};

		UINT m_BackBufferNum = 2;

		bool m_UseMSAA4X = false;

		D3D12_RESOURCE_STATES m_CurrentBackBufferResourceState = D3D12_RESOURCE_STATE_PRESENT;

		static DX12SwapChain s_SwapChain;

	};

}

#define TE_INSTANCE_API_DX12_SWAPCHAIN TruthEngine::API::DX12::DX12SwapChain::Get()