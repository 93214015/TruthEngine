#pragma once


namespace TruthEngine::API {

	class IDXGI {

	public:
		static inline IDXGI& GetIDXGI() 
		{
			static IDXGI s_IDXGI;

			return s_IDXGI; 
		}

		void Init();
		void CreateFactory();
		void EnumAdapters();
		inline IDXGIFactory7* GetDXGIFactory() noexcept { return m_Factory.Get(); };
		inline const std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter4>>& GetAdapters() { return m_Adapters; }
		inline bool CheckSupportAllowTearing()const noexcept { return m_SupportAllowTearing; }

	private:
		IDXGI();

		Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter4>> m_Adapters;
		bool m_IsInit = false;
		bool m_SupportAllowTearing = false;

	};

}

#define TE_INSTANCE_IDXGI TruthEngine::API::IDXGI::GetIDXGI()
