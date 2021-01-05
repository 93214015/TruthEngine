#pragma once
#include "API/DX12/DirectX12GraphicDevice.h"
#include "API/DX12/DirectX12CommandList.h"
#include "API/DX12/DirectX12DescriptorHeap.h"
#include "Core/Renderer/SwapChain.h"



namespace TruthEngine::API::DirectX12 {


	class DirectX12SwapChain : public TruthEngine::Core::SwapChain
	{

	public:
		static inline DirectX12SwapChain& GetInstance() { return s_SwapChain; }

		TE_RESULT Init(UINT clientWidth, UINT clientHeight, Core::Window* outputWindow, UINT backBufferNum = 2) override;

		void Release() override;

		TE_RESULT Resize(UINT width, UINT height, UINT backBufferNum) override;

		void InitRTVs(DescriptorHeapRTV* descHeap, Core::RenderTargetView* RTV);

		inline ID3D12Resource* GetBackBufferResource() const { return m_BackBuffers[GetCurrentFrameIndex()].Get(); };

		inline D3D12_RESOURCE_STATES GetBackBufferState() const { return m_CurrentBackBufferResourceState[GetCurrentFrameIndex()]; };

		inline void SetBackBufferState(D3D12_RESOURCE_STATES state) noexcept { m_CurrentBackBufferResourceState[GetCurrentFrameIndex()] = state; };

		void Present() override;


	protected:
		DirectX12SwapChain();

		void CreateSwapChain(HWND outputHWND);

		void CreateSwapChainRTVs(DescriptorHeapRTV* descHeap, Core::RenderTargetView* RTV);

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