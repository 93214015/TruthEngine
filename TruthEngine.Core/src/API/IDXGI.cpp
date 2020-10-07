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

		CreateDXGIFactory(IID_PPV_ARGS(m_Factory.GetAddressOf()));
	}

	void IDXGI::EnumAdapters()
	{
		int i = 0;

		ComPtr<IDXGIAdapter> adapter;


		auto hr = m_Factory->EnumAdapters(i, &adapter);

		while (hr != DXGI_ERROR_NOT_FOUND)
		{
			m_Adapters.push_back(adapter);
			i++;
			hr = m_Factory->EnumAdapters(i, &adapter);
		}
	}

	TruthEngine::API::IDXGI IDXGI::s_IDXGI;

}