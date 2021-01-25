#include "pch.h"
#include "DirectX12SwapChain.h"

#include "Core/Application.h"

#include "API/IDXGI.h"

using namespace Microsoft::WRL;


#ifdef TE_API_DX12

TruthEngine::Core::SwapChain* TruthEngine::Core::SwapChain::GetInstance()
{
	return &TruthEngine::API::DirectX12::DirectX12SwapChain::GetInstance();
}

#endif

namespace TruthEngine::API::DirectX12 {


	DirectX12SwapChain::DirectX12SwapChain() = default;

	TE_RESULT DirectX12SwapChain::Init(uint32_t clientWidth, uint32_t clientHeight, Core::Window* outputHWND, uint32_t backBufferNum)
	{

		m_BackBufferNum = backBufferNum;

		CreateSwapChain(static_cast<HWND>(outputHWND->GetNativeWindowHandle()));

		m_CurrentBackBufferResourceState.resize(backBufferNum, D3D12_RESOURCE_STATE_PRESENT);

		return TE_SUCCESSFUL;

	}

	void DirectX12SwapChain::Release()
	{
		m_BackBuffers.clear();
		m_SwapChain->Release();
	}

	void DirectX12SwapChain::InitRTVs(DescriptorHeapRTV* descHeap, Core::RenderTargetView* RTV)
	{
		CreateSwapChainRTVs(descHeap, RTV);
	}

	void DirectX12SwapChain::Present()
	{
		UINT presentFlags = (TE_INSTANCE_IDXGI.CheckSupportAllowTearing() && TE_INSTANCE_APPLICATION->GetWindowMode()) ? DXGI_PRESENT_ALLOW_TEARING : 0;

		if (FAILED(m_SwapChain->Present(0, presentFlags)))
		{
			TE_ASSERT_CORE(false, "SwapChain Present was failed");
		}
	}

	void DirectX12SwapChain::CreateSwapChain(HWND outputHWND)
	{
		{

			COMPTR<IDXGISwapChain1> swapChain1;

			DXGI_SWAP_CHAIN_DESC1 desc1;
			desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			desc1.BufferCount = m_BackBufferNum;
			desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			//desc1.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			desc1.Flags =  (TE_INSTANCE_IDXGI.CheckSupportAllowTearing() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0) | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			desc1.Format = m_BackbufferFormat;
			desc1.Width = TE_INSTANCE_APPLICATION->GetClientWidth();
			desc1.Height = TE_INSTANCE_APPLICATION->GetClientHeight();
			desc1.SampleDesc.Count = m_UseMSAA4X ? 4 : 1;
			desc1.SampleDesc.Quality = m_UseMSAA4X ? m_MSAAQualityLevels.NumQualityLevels - 1 : 0;
			desc1.Scaling = DXGI_SCALING_NONE;
			desc1.Stereo = FALSE;
			desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

			auto r = TE_INSTANCE_IDXGI.GetDXGIFactory()->CreateSwapChainForHwnd(TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT->m_CommandQueue.Get(), outputHWND, &desc1, NULL, NULL, swapChain1.GetAddressOf());

			TE_ASSERT_CORE(SUCCEEDED(r), "API::DirectX12  Creation of 'SwapChain' factory is failed!");

			swapChain1->QueryInterface(m_SwapChain.ReleaseAndGetAddressOf());

		}

		m_SwapChain->SetMaximumFrameLatency(m_BackBufferNum);

		m_BackBuffers.clear();
		m_BackBuffers.resize(m_BackBufferNum);

		for (uint32_t i = 0; i < m_BackBufferNum; i++)
		{
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffers[i].GetAddressOf()));
		}
	}

	void DirectX12SwapChain::CreateSwapChainRTVs(DescriptorHeapRTV* descHeap, Core::RenderTargetView* RTV)
	{

		if (RTV->ViewIndex == -1)
		{
			RTV->ViewIndex = descHeap->GetCurrentIndex();

			for (uint32_t i = 0; i < m_BackBufferNum; i++)
			{
				ComPtr<ID3D12Resource> buffer;
				m_SwapChain->GetBuffer(i, IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf()));
				descHeap->AddDescriptor(buffer.Get());
			}

		}
		else
		{
			for (uint32_t i = 0; i < m_BackBufferNum; i++)
			{
				ComPtr<ID3D12Resource> buffer;
				m_SwapChain->GetBuffer(i, IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf()));
				descHeap->ReplaceDescriptor(buffer.Get(), (RTV->ViewIndex + i) );
			}
		}
	}

	void DirectX12SwapChain::CheckDeviceFeatures()
	{
		m_MSAAQualityLevels.Format = m_BackbufferFormat;
		m_MSAAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		m_MSAAQualityLevels.NumQualityLevels = 0;
		m_MSAAQualityLevels.SampleCount = 4;

		TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS
			, &m_MSAAQualityLevels
			, sizeof(m_MSAAQualityLevels));


		if (m_MSAAQualityLevels.NumQualityLevels < 0)
		{
			m_UseMSAA4X = false;
		}
	}

	TE_RESULT DirectX12SwapChain::Resize(uint32_t width, uint32_t height, uint32_t backBufferNum)
	{
		m_BackBufferNum = backBufferNum;

		m_SwapChain->SetMaximumFrameLatency(backBufferNum);

// 		m_BackBuffers.clear();
// 		m_BackBuffers.resize(backBufferNum);

		for (uint32_t i = 0; i < backBufferNum; i++)
		{
			m_BackBuffers[i].Reset();
		}

		auto hr = m_SwapChain->ResizeBuffers(backBufferNum, width, height, m_BackbufferFormat, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);

		for (uint32_t i = 0; i < backBufferNum; i++)
		{
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffers[i].ReleaseAndGetAddressOf()));
		}

		BOOL _fullscreened = false;
		m_SwapChain->GetFullscreenState(&_fullscreened, nullptr);
		TE_INSTANCE_APPLICATION->SetWindowMode((bool)!_fullscreened);

		if (SUCCEEDED(hr))
			return TE_SUCCESSFUL;
		else
			return TE_FAIL;
	}

	uint8_t DirectX12SwapChain::GetCurrentFrameIndex() const
	{
		return static_cast<uint8_t>(m_SwapChain->GetCurrentBackBufferIndex());
	}

	TruthEngine::API::DirectX12::DirectX12SwapChain DirectX12SwapChain::s_SwapChain;

}