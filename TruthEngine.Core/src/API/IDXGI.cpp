#include "pch.h"
#include "IDXGI.h"
#include "Core/Application.h"

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

		m_Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &m_SupportAllowTearing, sizeof(m_SupportAllowTearing));

		if (m_SupportAllowTearing)
		{
			// When tearing support is enabled we will handle ALT+Enter key presses in the
		// window message loop rather than let DXGI handle it by calling SetFullscreenState.
			m_Factory->MakeWindowAssociation(static_cast<HWND>(TE_INSTANCE_APPLICATION->GetWindow()->GetNativeWindowHandle()), DXGI_MWA_NO_ALT_ENTER);
		}

	}

	void IDXGI::EnumAdapters()
	{
		int i = 0;

		ComPtr<IDXGIAdapter4> adapter;

		while (m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC3 _desc;
			adapter->GetDesc3(&_desc);

			m_Adapters.push_back(adapter);
			i++;
		}
	}

	IDXGI::IDXGI()
	{
		CreateFactory();
		EnumAdapters();

		m_IsInit = true;
	}

}