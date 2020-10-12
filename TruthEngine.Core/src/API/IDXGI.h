#pragma once


namespace TruthEngine::API {

	class IDXGI {

	public:
		static inline IDXGI& GetIDXGI() { return s_IDXGI; }

		IDXGI() { Init(); }

		void Init();
		void CreateFactory();
		void EnumAdapters();
		inline IDXGIFactory7* GetDXGIFactory() noexcept { return m_Factory.Get(); };
		inline const std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter4>>& GetAdapters() { return m_Adapters; }

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter4>> m_Adapters;
		bool m_IsInit;

		static IDXGI s_IDXGI;
	};

}

#define TE_INSTANCE_IDXGI TruthEngine::API::IDXGI::GetIDXGI()
