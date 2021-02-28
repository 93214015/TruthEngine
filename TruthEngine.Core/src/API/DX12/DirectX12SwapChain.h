#pragma once
#include "API/DX12/DirectX12GraphicDevice.h"
#include "API/DX12/DirectX12CommandList.h"
#include "API/DX12/DirectX12DescriptorHeap.h"
#include "Core/Renderer/SwapChain.h"



namespace TruthEngine::API::DirectX12 {


	class DirectX12SwapChain : public TruthEngine::SwapChain
	{

	public:
		static inline DirectX12SwapChain& GetInstance() { return s_SwapChain; }

		TE_RESULT Init(uint32_t clientWidth, uint32_t clientHeight, Window* outputWindow, uint32_t backBufferNum = 2) override;

		void Release() override;

		TE_RESULT Resize(uint32_t width, uint32_t height, uint32_t backBufferNum) override;

		uint8_t GetCurrentFrameIndex() const override;

		void InitRTVs(DescriptorHeapRTV* descHeap, RenderTargetView* RTV);

		inline ID3D12Resource* GetBackBufferResource() const { return m_BackBuffers[GetCurrentFrameIndex()].Get(); };

		inline D3D12_RESOURCE_STATES GetBackBufferState() const { return m_CurrentBackBufferResourceState[GetCurrentFrameIndex()]; };

		inline void SetBackBufferState(D3D12_RESOURCE_STATES state) noexcept { m_CurrentBackBufferResourceState[GetCurrentFrameIndex()] = state; };

		IDXGISwapChain4* GetNativeObject() const noexcept { return m_SwapChain.Get(); }

		void Present() override;


	protected:
		DirectX12SwapChain();

		void CreateSwapChain(HWND outputHWND);

		void CreateSwapChainRTVs(DescriptorHeapRTV* descHeap, RenderTargetView* RTV);

		void CheckDeviceFeatures();

	protected:
		
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain = nullptr;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_BackBuffers{};

		DXGI_FORMAT m_BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS m_MSAAQualityLevels{};
			

		std::vector<D3D12_RESOURCE_STATES> m_CurrentBackBufferResourceState;

		static DirectX12SwapChain s_SwapChain;

	};

}

#define TE_INSTANCE_API_DX12_SWAPCHAIN TruthEngine::API::DirectX12::DirectX12SwapChain::GetInstance()