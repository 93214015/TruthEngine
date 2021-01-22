#include "pch.h"
#include "IDXGI.h"

using namespace Microsoft::WRL;

namespace TruthEngine::API {



	void IDXGI::Init()
	{
		if (m_IsInit)
			return;

		CreateFactory();
		EnumAdapters();

		m_IsInit = true;
	}

	void IDXGI::CreateFactory()
	{
		UINT dxgiFactoryFlags = 0;

#if defined(TE_DEBUG)

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#endif
		{
			COMPTR<IDXGIFactory3> factory3;

			CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory3.GetAddressOf()));

			factory3->QueryInterface(m_Factory.ReleaseAndGetAddressOf());
		}

	}

	void IDXGI::EnumAdapters()
	{
		int i = 0;

		ComPtr<IDXGIAdapter4> adapter;


		auto hr = m_Factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,  IID_PPV_ARGS(&adapter));

		while (hr != DXGI_ERROR_NOT_FOUND)
		{
			m_Adapters.push_back(adapter);
			i++;
			hr = m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
		}
	}

	IDXGI::IDXGI()
	{
		CreateFactory();
		EnumAdapters();

		m_IsInit = true;
	}

}