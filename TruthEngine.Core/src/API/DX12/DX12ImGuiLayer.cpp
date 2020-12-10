#include "pch.h"
#include "DX12ImGuiLayer.h"

#include "Core/Application.h"

#include "Platform/Windows/WindowsWindow.h"

#include "API/IDXGI.h"
#include "API/DX12/DX12GraphicDevice.h"
#include "API/DX12/DX12SwapChain.h"



#ifdef TE_API_DX12

//TruthEngine::Core::ImGuiLayer* TruthEngine::Core::ImGuiLayer::Factory() {
//
//	return new API::DirectX12::DX12ImGuiLayer();
//};

namespace TruthEngine::API::DirectX12 {



	void DX12ImGuiLayer::OnAttach()
	{
		TE_TIMER_SCOPE_FUNC;

		m_CommandList = std::make_shared<DX12CommandList>(&TE_INSTANCE_API_DX12_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, nullptr, nullptr);

		m_DescHeapSRV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, TE_INSTANCE_APPLICATION->GetFramesInFlightNum());
		m_DescHeapRTV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, TE_INSTANCE_APPLICATION->GetFramesInFlightNum());

		TE_INSTANCE_API_DX12_SWAPCHAIN.InitRTVs(&m_DescHeapRTV);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		HWND hwnd = static_cast<HWND>(TE_INSTANCE_APPLICATION->GetWindow()->GetNativeWindowHandle());
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetDevice()
			, TE_INSTANCE_APPLICATION->GetFramesInFlightNum()
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, m_DescHeapSRV.GetDescriptorHeap()
			, m_DescHeapSRV.GetCPUHandleLast()
			, m_DescHeapSRV.GetGPUHandleLast());

		

	}

	void DX12ImGuiLayer::OnDetach()
	{
		TE_TIMER_SCOPE_FUNC;

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();


		m_DescHeapSRV.Release();
		m_CommandList->Release();
	}

	void DX12ImGuiLayer::Begin()
	{
		TE_TIMER_SCOPE_FUNC;

		// Our state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}

	void DX12ImGuiLayer::End()
	{
		TE_TIMER_SCOPE_FUNC;

		const uint32_t currentFrameIndex = TE_INSTANCE_APPLICATION->GetCurrentFrameIndex();

		m_CommandList->GetCommandAllocator()->Reset();
		auto dx12CmdList = m_CommandList->GetNativeObject();
		dx12CmdList->Reset(m_CommandList->GetCommandAllocator()->GetNativeObject(), nullptr);

		auto& sc = TE_INSTANCE_API_DX12_SWAPCHAIN;

		ID3D12DescriptorHeap* descheaps[] = { m_DescHeapSRV.GetDescriptorHeap() };
		if (sc.GetBackBufferState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
		{
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(sc.GetBackBufferResource(), sc.GetBackBufferState(), D3D12_RESOURCE_STATE_RENDER_TARGET);
			dx12CmdList->ResourceBarrier(1, &barrier);
		}
		dx12CmdList->SetDescriptorHeaps(1, descheaps);
		dx12CmdList->OMSetRenderTargets(1, &m_DescHeapRTV.GetCPUHandle(currentFrameIndex), FALSE, NULL);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12CmdList);

		/*barrier = CD3DX12_RESOURCE_BARRIER::Transition(sc.GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		dx12CmdList->ResourceBarrier(1, &barrier);*/


		TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT.ExecuteCommandList(*m_CommandList);


		auto& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, dx12CmdList);
		}
	}

}

#endif
