#include "pch.h"
#include "SwapChain.h"
#include "Application.h"
#include "API/IDXGI.h"
#include "Core/GPU/GDevice.h"

using namespace Microsoft::WRL;

namespace TruthEngine::API::DX12 {


	SwapChain::SwapChain() = default;

	TE_RESULT SwapChain::Init(UINT clientWidth, UINT clientHeight, HWND* outputHWND, UINT backBufferNum)
	{
		m_BackBufferNum = backBufferNum;

		CreateSwapChain(outputHWND);

		return TE_SUCCESSFUL;

	}

	TE_RESULT SwapChain::InitRTVs(DescriptorHeapRTV* descHeap)
	{
		m_DescHeapRTV = descHeap;
		CreateSwapChainRTVs();
		return TE_SUCCESSFUL;
	}

	void SwapChain::Present()
	{
		m_SwapChain->Present(1, 0);
	}

	void SwapChain::CreateSwapChain(HWND* outputHWND)
	{
		{

			COMPTR<IDXGISwapChain1> swapChain1;

			DXGI_SWAP_CHAIN_DESC1 desc1;
			desc1.AlphaMode = DXGI_ALPHA_MODE_STRAIGHT;
			desc1.BufferCount = m_BackBufferNum;
			desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc1.Flags = 0;
			desc1.Format = m_BackbufferFormat;
			desc1.Width = TE_INSTANCE_APPLICATION.GetClientWidth();
			desc1.Height = TE_INSTANCE_APPLICATION.GetClientHeight();
			desc1.SampleDesc.Count = m_UseMSAA4X ? 4 : 1;
			desc1.SampleDesc.Quality = m_UseMSAA4X ? m_MSAAQualityLevels.NumQualityLevels - 1 : 0;
			desc1.Scaling = DXGI_SCALING_NONE;
			desc1.Stereo = FALSE;
			desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

			TE_ASSERT_CORE(TE_SUCCEEDED(TE_INSTANCE_IDXGI.GetDXGIFactory()->CreateSwapChainForHwnd(TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT, *outputHWND, &desc1, NULL, NULL, &swapChain1)), "API::DX12 Creation of 'SwapChain' factory is failed!");

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

	void SwapChain::CreateSwapChainRTVs()
	{
		m_BackBufferDescOffset = m_DescHeapRTV->GetCurrentIndex();

		for (UINT i = 0; i < m_BackBufferNum; i++)
		{
			ComPtr<ID3D12Resource> buffer;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf()));
			m_DescHeapRTV->AddDescriptor(buffer.Get());
		}
	}

	void SwapChain::CheckDeviceFeatures()
	{
		m_MSAAQualityLevels.Format = m_BackbufferFormat;
		m_MSAAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		m_MSAAQualityLevels.NumQualityLevels = 0;
		m_MSAAQualityLevels.SampleCount = 4;

		TE_INSTANCE_API_DX12_GDEVICE.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS
			, &m_MSAAQualityLevels
			, sizeof(m_MSAAQualityLevels));


		if (m_MSAAQualityLevels.NumQualityLevels < 0)
		{
			m_UseMSAA4X = false;
		}
	}

	TE_RESULT SwapChain::Resize(UINT width, UINT height, UINT backBufferNum)
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

}