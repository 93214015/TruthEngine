#include "pch.h"
#include "ImGuiLayer.h"

#include "Application.h"
#include "Platform/Windows/WindowWindows.h"
#include "Core/GPU/GDevice.h"

namespace TruthEngine::Core {


	void ImGuiLayer::OnAttach()
	{


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();


#ifdef TE_PLATFORM_WINDOWS
		auto window = reinterpret_cast<TruthEngine::Platforms::Windows::WindowWindows*>(TE_INSTANCE_APPLICATION.GetWindow());
		ImGui_ImplWin32_Init(window->GetHandle());
#endif
#ifdef TE_API_DX12
		m_DescHeapSRV.Init(TE_INSTANCE_API_DX12_GDEVICE.GetDevice(), 1);
		ImGui_ImplDX12_Init(TE_INSTANCE_API_DX12_GDEVICE.GetDevice()
			, 2
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, m_DescHeapSRV.GetDescriptorHeap()
			, m_DescHeapSRV.GetCPUHandleLast()
			, m_DescHeapSRV.GetGPUHandleLast());
#endif

		// Our state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	}

	void ImGuiLayer::OnDetach()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ImGuiLayer::OnUpdate(double deltaFrameTime)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ImGuiLayer::OnImGuiRender()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ImGuiLayer::OnEvent(const Event& event)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}