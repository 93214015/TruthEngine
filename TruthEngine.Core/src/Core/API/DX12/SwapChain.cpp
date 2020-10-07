#include "pch.h"
#include "SwapChain.h"
#include "Application.h"
#include "Core/API/IDXGI.h"

using namespace Microsoft::WRL;

namespace TruthEngine::Core::API::DX12 {


	SwapChain::SwapChain() = default;

	TE_RESULT SwapChain::Init(UINT clientWidth, UINT clientHeight, HWND* outputHWND)
	{

		CreateSwapChain(outputHWND);

		return TE_SUCCESFUL;

	}

	TE_RESULT SwapChain::InitRTVs(DescriptorHeapRTV* descHeap)
	{
		m_DescHeapRTV = descHeap;
		CreateSwapChainRTVs();
		return TE_SUCCESFUL;
	}

	void SwapChain::Present()
	{
		m_SwapChain->Present(1, 0);

		m_BackBufferIndex ^= 1;
	}

	void SwapChain::CreateSwapChain(HWND* outputHWND)
	{
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = TE_INSTANCE_APPLICATION.GetClientWidth();
		desc.BufferDesc.Height = TE_INSTANCE_APPLICATION.GetClientHeight();
		desc.BufferDesc.Format = m_BackbufferFormat;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferCount = m_BackBufferNum;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = 0;
		desc.SampleDesc.Count = m_UseMSAA4X ? 4 : 1;
		desc.SampleDesc.Quality = m_UseMSAA4X ? m_MSAAQualityLevels.NumQualityLevels - 1 : 0;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Windowed = true;
		desc.OutputWindow = *outputHWND;

		TE_INSTANCE_IDXGI.GetDXGIFactory()->CreateSwapChain(TE_INSTANCE_COMMANDQUEUEDIRECT, &desc, m_SwapChain.ReleaseAndGetAddressOf());

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

		TE_INSTANCE_GDEVICEDX12.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS
			, &m_MSAAQualityLevels
			, sizeof(m_MSAAQualityLevels));


		if (m_MSAAQualityLevels.NumQualityLevels < 0)
		{
			m_UseMSAA4X = false;
		}
	}

}