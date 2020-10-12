#include "pch.h"
#include "API/DX12/ImGuiLayerDX12.h"

#include "Application.h"
#include "Platform/Windows/WindowWindows.h"
#include "API/DX12/GDeviceDX12.h"
#include "API/IDXGI.h"
#include "API/DX12/SwapChain.h"


#ifdef TE_API_DX12

namespace TruthEngine::API::DX12 {



	void ImGuiLayerDX12::OnAttach()
	{
		m_CommandList.Init(TE_INSTANCE_APPLICATION.GetFramesInFlightNum(), D3D12_COMMAND_LIST_TYPE_DIRECT, TE_INSTANCE_API_DX12_GDEVICE);

		m_DescHeapSRV.Init(TE_INSTANCE_API_DX12_GDEVICE.GetDevice(), TE_INSTANCE_APPLICATION.GetFramesInFlightNum());

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();


		auto window = reinterpret_cast<TruthEngine::Platforms::Windows::WindowWindows*>(TE_INSTANCE_APPLICATION.GetWindow());
		ImGui_ImplWin32_Init(window->GetHandle());
		ImGui_ImplDX12_Init(TE_INSTANCE_API_DX12_GDEVICE.GetDevice()
			, 2
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, m_DescHeapSRV.GetDescriptorHeap()
			, m_DescHeapSRV.GetCPUHandleLast()
			, m_DescHeapSRV.GetGPUHandleLast());

		

	}

	void ImGuiLayerDX12::OnDetach()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();


		m_DescHeapSRV.Release();
		m_CommandList.Release();
	}

	void ImGuiLayerDX12::OnUpdate(double deltaFrameTime)
	{

	}

	void ImGuiLayerDX12::OnImGuiRender()
	{

	}

	void ImGuiLayerDX12::OnEvent(TruthEngine::Core::Event& event)
	{

	}

	void ImGuiLayerDX12::Begin()
	{


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

	void ImGuiLayerDX12::End()
	{
		const uint32_t currentFrameIndex = TE_INSTANCE_APPLICATION.GetCurrentFrameIndex();

		m_CommandList.Reset(currentFrameIndex, nullptr);

		ID3D12DescriptorHeap* descheaps[] = { m_DescHeapSRV.GetDescriptorHeap() };
		m_CommandList->SetDescriptorHeaps(1, descheaps);
		m_CommandList->OMSetRenderTargets(1, &TE_INSTANCE_API_DX12_SWAPCHAIN.GetCurrentBackBufferRTV(), FALSE, NULL);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList.Get());

		TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT.ExecuteCommandList(m_CommandList);
	}

}

#endif
