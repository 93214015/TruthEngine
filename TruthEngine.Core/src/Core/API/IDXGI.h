#pragma once


namespace TruthEngine::Core::API {

	class IDXGI {

	public:
		static inline IDXGI& GetIDXGI() { return s_IDXGI; }

		IDXGI() { Init(); }

		void Init();
		void CreateFactory();
		void EnumAdapters();
		inline IDXGIFactory* GetDXGIFactory() noexcept { return m_Factory.Get(); };
		inline const std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>>& GetAdapters() { return m_Adapters; }

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory> m_Factory;
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>> m_Adapters;
		bool m_IsInit;

		static IDXGI s_IDXGI;
	};

}

#define TE_INSTANCE_IDXGI TruthEngine::Core::API::IDXGI::GetIDXGI()
