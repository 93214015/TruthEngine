#include "pch.h"
#include "DX12SwapChain.h"

#include "Core/Application.h"

#include "API/IDXGI.h"

using namespace Microsoft::WRL;


#ifdef TE_API_DX12

TruthEngine::Core::SwapChain* TruthEngine::Core::SwapChain::GetInstance()
{
	return &TruthEngine::API::DirectX12::DX12SwapChain::GetInstance();
}

#endif

namespace TruthEngine::API::DirectX12 {


	DX12SwapChain::DX12SwapChain() = default;

	TE_RESULT DX12SwapChain::Init(UINT clientWidth, UINT clientHeight, HWND outputHWND, UINT backBufferNum)
	{

		m_BackBufferNum = backBufferNum;

		CreateSwapChain(outputHWND);

		m_CurrentBackBufferResourceState.resize(backBufferNum, D3D12_RESOURCE_STATE_PRESENT);

		return TE_SUCCESSFUL;

	}

	uint32_t DX12SwapChain::InitRTVs(DescriptorHeapRTV* descHeap)
	{
		return CreateSwapChainRTVs(descHeap);
	}

	void DX12SwapChain::Present()
	{
		m_SwapChain->Present(1, 0);
	}

	void DX12SwapChain::CreateSwapChain(HWND outputHWND)
	{
		{

			COMPTR<IDXGISwapChain1> swapChain1;

			DXGI_SWAP_CHAIN_DESC1 desc1;
			desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			desc1.BufferCount = m_BackBufferNum;
			desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc1.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			desc1.Format = m_BackbufferFormat;
			desc1.Width = TE_INSTANCE_APPLICATION->GetClientWidth();
			desc1.Height = TE_INSTANCE_APPLICATION->GetClientHeight();
			desc1.SampleDesc.Count = m_UseMSAA4X ? 4 : 1;
			desc1.SampleDesc.Quality = m_UseMSAA4X ? m_MSAAQualityLevels.NumQualityLevels - 1 : 0;
			desc1.Scaling = DXGI_SCALING_NONE;
			desc1.Stereo = FALSE;
			desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

			auto r = TE_INSTANCE_IDXGI.GetDXGIFactory()->CreateSwapChainForHwnd(TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT->m_CommandQueue.Get(), outputHWND, &desc1, NULL, NULL, &swapChain1);

			TE_ASSERT_CORE(r, "API::DirectX12  Creation of 'SwapChain' factory is failed!");

			swapChain1->QueryInterface(m_SwapChain.ReleaseAndGetAddressOf());

		}

		m_SwapChain->SetMaximumFrameLatency(m_BackBufferNum);

		m_BackBuffers.clear();
		m_BackBuffers.resize(m_BackBufferNum);

		for (UINT i = 0; i < m_BackBufferNum; i++)
		{
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffers[i].GetAddressOf()));
		}
	}

	uint32_t DX12SwapChain::CreateSwapChainRTVs(DescriptorHeapRTV* descHeap)
	{
		auto index = descHeap->GetCurrentIndex();

		for (UINT i = 0; i < m_BackBufferNum; i++)
		{
			ComPtr<ID3D12Resource> buffer;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf()));
			descHeap->AddDescriptor(buffer.Get());
		}

		return index;
	}

	void DX12SwapChain::CheckDeviceFeatures()
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

	TE_RESULT DX12SwapChain::Resize(UINT width, UINT height, UINT backBufferNum)
	{
		m_BackBufferNum = backBufferNum;

		m_SwapChain->SetMaximumFrameLatency(backBufferNum);

		m_BackBuffers.clear();
		m_BackBuffers.resize(backBufferNum);

		auto hr = m_SwapChain->ResizeBuffers(backBufferNum, width, height, m_BackbufferFormat, 0);

		for (UINT i = 0; i < backBufferNum; i++)
		{
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffers[i].ReleaseAndGetAddressOf()));
		}

		if (SUCCEEDED(hr))
			return TE_SUCCESSFUL;
		else
			return TE_FAIL;
	}

	TruthEngine::API::DirectX12::DX12SwapChain DX12SwapChain::s_SwapChain;

}