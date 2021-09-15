#include "pch.h"
#include "DirectX12ImGuiLayer.h"

#include "Core/Application.h"
#include "Core/Entity/Model/ModelManager.h"

#include "Platform/Windows/WindowsWindow.h"

#include "API/IDXGI.h"
#include "API/DX12/DirectX12GraphicDevice.h"
#include "API/DX12/DirectX12SwapChain.h"
#include "API/DX12/DirectX12BufferManager.h"




#ifdef TE_API_DX12

//TruthEngine::ImGuiLayer* TruthEngine::ImGuiLayer::Factory() {
//
//	return new API::DirectX12::DX12ImGuiLayer();
//};

namespace TruthEngine::API::DirectX12 {



	void DirectX12ImGuiLayer::OnAttach()
	{
		TE_TIMER_SCOPE_FUNC;

		m_D3D12Viewport = D3D12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), 0.0f, 1.0f };
		m_D3D12ViewRect = D3D12_RECT{ static_cast<long>(0.0f), static_cast<long>(0.0f), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()),  static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()) };

		m_CommandList.resize(TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum());

		for (uint8_t i = 0; i < TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum(); ++i)
			m_CommandList[i] = std::make_shared<DirectX12CommandList>(&TE_INSTANCE_API_DX12_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, nullptr, nullptr, TE_IDX_RENDERPASS::NONE, TE_IDX_SHADERCLASS::NONE, i);

		auto dx12bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);

		//m_DescHeapSRV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, TE_INSTANCE_APPLICATION->GetFramesInFlightNum());
		m_DescHeapSRV = &dx12bufferManager->GetDescriptorHeapSRV();
		m_DescHeapRTV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum());


		auto desc = CD3DX12_RESOURCE_DESC1::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, static_cast<uint64_t>(TE_INSTANCE_APPLICATION->GetClientWidth()), TE_INSTANCE_APPLICATION->GetClientHeight());
		TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc
			, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			, nullptr
			, nullptr
			, IID_PPV_ARGS(m_TextureMultiSampleResolved.ReleaseAndGetAddressOf()));

		TE_INSTANCE_API_DX12_SWAPCHAIN.InitRTVs(&m_DescHeapRTV, &m_RTVBackBuffer);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 13.0f);


		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		m_RenderTargetScreenBuffer = dx12bufferManager->GetRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer);

		m_D3D12Resource_ScreenBuffer = dx12bufferManager->GetResource(m_RenderTargetScreenBuffer);

		if ( false /*Settings::Graphics::IsEnabledMSAA()*/)
		{
			if (m_SRVIndexScreenBuffer == -1)
			{
				m_SRVIndexScreenBuffer = m_DescHeapSRV->AddDescriptorSRV(m_TextureMultiSampleResolved.Get(), nullptr);
			}
			else
			{
				m_DescHeapSRV->ReplaceDescriptorSRV(m_TextureMultiSampleResolved.Get(), nullptr, m_SRVIndexScreenBuffer);
			}
		}
		else
		{
			if (m_SRVIndexScreenBuffer == -1)
			{
				m_SRVIndexScreenBuffer = m_DescHeapSRV->AddDescriptorSRV(m_D3D12Resource_ScreenBuffer, nullptr);
			}
			else
			{
				m_DescHeapSRV->ReplaceDescriptorSRV(m_D3D12Resource_ScreenBuffer, nullptr, m_SRVIndexScreenBuffer);
			}
		}

		HWND hwnd = static_cast<HWND>(TE_INSTANCE_APPLICATION->GetWindow()->GetNativeWindowHandle());
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetDevice()
			, TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum()
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, m_DescHeapSRV->GetDescriptorHeap()
			, m_DescHeapSRV->GetCPUHandle(0)
			, m_DescHeapSRV->GetGPUHandle(0));


		auto lambda_OnTextureResize = [this](Event& event) { OnRendererTextureResize(static_cast<EventRendererTextureResize&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererTextureResize, lambda_OnTextureResize);

		auto lambda_OnViewportResize = [this](Event& event) { OnRendererViewportResize(static_cast<EventRendererViewportResize&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, lambda_OnViewportResize);

	}

	void DirectX12ImGuiLayer::OnDetach()
	{
		//TE_TIMER_SCOPE_FUNC;

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		for (auto& cmd : m_CommandList)
			cmd->Release();
	}

	void DirectX12ImGuiLayer::Begin()
	{
		//TE_TIMER_SCOPE_FUNC;

		// Our state
		bool show_demo_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();


		//Set DockPanel
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (m_ShowDemoWindow)
			ImGui::ShowDemoWindow(&show_demo_window);

		//// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		//{
		//	static float f = 0.0f;
		//	static int counter = 0;

		//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

		//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//		counter++;
		//	ImGui::SameLine();
		//	ImGui::Text("counter = %d", counter);

		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	ImGui::End();
		//}

		/*ImGui::Begin("OpenFile");
		if (ImGui::Button("Open File Dialog"))
		{
			const std::vector<const char*> fileExtensions = { ".obj", ".fbx" };
			OpenFileDialog("Open Model", fileExtensions);
		}
		m_FileBrowser.Display();
		ImGui::End();
		if (CheckFileDialog())
		{
			GraphicDevice::GetPrimaryDevice()->WaitForGPU();
			m_CommandList->WaitToFinish();
			ModelManager::GetInstance()->ImportModel(GetActiveScene(), m_SelectedFile.c_str());
		}*/


	}

	void DirectX12ImGuiLayer::End()
	{
			//TE_TIMER_SCOPE_FUNC;

			WindowMaterialTextures();

			const uint32_t currentFrameIndex = TE_INSTANCE_APPLICATION->GetCurrentFrameIndex();

			m_CommandList[currentFrameIndex]->GetCommandAllocator()->Reset();
			auto dx12CmdList = m_CommandList[currentFrameIndex]->GetNativeObject();
			dx12CmdList->Reset(m_CommandList[currentFrameIndex]->GetCommandAllocator()->GetNativeObject(), nullptr);

			auto& sc = TE_INSTANCE_API_DX12_SWAPCHAIN;

			CD3DX12_RESOURCE_BARRIER barriers[3];
			uint32_t barrierNum = 0;

			if (sc.GetBackBufferState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
			{
				barriers[barrierNum] = CD3DX12_RESOURCE_BARRIER::Transition(sc.GetBackBufferResource(), sc.GetBackBufferState(), D3D12_RESOURCE_STATE_RENDER_TARGET);
				sc.SetBackBufferState(D3D12_RESOURCE_STATE_RENDER_TARGET);
				barrierNum++;
			}
			/*if (m_RenderTargetScreenBuffer->GetState() != TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE)
			{
				barriers[barrierNum] = CD3DX12_RESOURCE_BARRIER::Transition(m_D3D12Resource_ScreenBuffer, static_cast<D3D12_RESOURCE_STATES>(m_RenderTargetScreenBuffer->GetState()), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				m_RenderTargetScreenBuffer->SetState(TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE);
				barrierNum++;
			}*/

			if (false /*Settings::Graphics::IsEnabledMSAA()*/)
			{
				if (m_RenderTargetScreenBuffer->GetState() != TE_RESOURCE_STATES::RESOLVE_SOURCE)
				{
					barriers[barrierNum] = CD3DX12_RESOURCE_BARRIER::Transition(m_D3D12Resource_ScreenBuffer, static_cast<D3D12_RESOURCE_STATES>(m_RenderTargetScreenBuffer->GetState()), D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
					m_RenderTargetScreenBuffer->SetState(TE_RESOURCE_STATES::RESOLVE_SOURCE);
					barrierNum++;

				}

				barriers[barrierNum] = CD3DX12_RESOURCE_BARRIER::Transition(m_TextureMultiSampleResolved.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RESOLVE_DEST);
				barrierNum++;
			}
			else
			{
				if (m_RenderTargetScreenBuffer->GetState() != TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE)
				{
					barriers[barrierNum] = CD3DX12_RESOURCE_BARRIER::Transition(m_D3D12Resource_ScreenBuffer, static_cast<D3D12_RESOURCE_STATES>(m_RenderTargetScreenBuffer->GetState()), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
					m_RenderTargetScreenBuffer->SetState(TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE);
					barrierNum++;

				}
			}

			if (barrierNum > 0)
			{
				dx12CmdList->ResourceBarrier(barrierNum, barriers);
			}

			CD3DX12_RESOURCE_BARRIER _barriers[1];
			if (false /*Settings::Graphics::IsEnabledMSAA()*/)
			{
				dx12CmdList->ResolveSubresource(m_TextureMultiSampleResolved.Get(), 0, m_D3D12Resource_ScreenBuffer, 0, DXGI_FORMAT_R8G8B8A8_UNORM);

				_barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_TextureMultiSampleResolved.Get(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

				dx12CmdList->ResourceBarrier(1, _barriers);
			}

			auto& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(TE_INSTANCE_APPLICATION->GetClientWidth(), TE_INSTANCE_APPLICATION->GetClientHeight());

			ID3D12DescriptorHeap* descheaps[] = { m_DescHeapSRV->GetDescriptorHeap() };
			dx12CmdList->RSSetViewports(1, &m_D3D12Viewport);
			dx12CmdList->RSSetScissorRects(1, &m_D3D12ViewRect);
			dx12CmdList->SetDescriptorHeaps(1, descheaps);
			dx12CmdList->OMSetRenderTargets(1, &m_DescHeapRTV.GetCPUHandle(currentFrameIndex), FALSE, NULL);
			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12CmdList);


			TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT->ExecuteCommandList(m_CommandList[currentFrameIndex].get());


			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault(NULL, dx12CmdList);
			}
	}

	/*void DirectX12ImGuiLayer::RenderSceneViewport()
	{
		ImGui::Begin("SceneViewport", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
		auto viewportSize = ImGui::GetContentRegionAvail();
		if (viewportSize.x != TE_INSTANCE_APPLICATION->GetSceneViewportWidth() || viewportSize.y != TE_INSTANCE_APPLICATION->GetSceneViewportHeight())
		{
			if (viewportSize.x > 1 && viewportSize.y > 1)
			{
				TE_INSTANCE_APPLICATION->ResizeSceneViewport(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
				OnSceneViewportResize();
			}
		}

		ImGui::Image((ImTextureID)m_DescHeapSRV.GetGPUHandle(m_SRVIndexScreenBuffer).ptr, viewportSize);

		ImGui::End();
	}*/

	void DirectX12ImGuiLayer::RenderSceneViewport(const ImVec2& viewportSize)
	{
		ImGui::Image((ImTextureID)m_DescHeapSRV->GetGPUHandle(m_SRVIndexScreenBuffer).ptr, viewportSize);
	}

	void DirectX12ImGuiLayer::OnRendererTextureResize(const EventRendererTextureResize& event)
	{
		if (event.GetIDX() == TE_IDX_GRESOURCES::Texture_RT_BackBuffer)
		{
			TE_INSTANCE_API_DX12_SWAPCHAIN.InitRTVs(&m_DescHeapRTV, &m_RTVBackBuffer);

			m_D3D12Viewport = D3D12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), 0.0f, 1.0f };
			m_D3D12ViewRect = D3D12_RECT{ static_cast<long>(0.0f), static_cast<long>(0.0f), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()),  static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()) };
		}
	}

	void DirectX12ImGuiLayer::OnRendererViewportResize(const EventRendererViewportResize& event)
	{
		if (false /*Settings::Graphics::IsEnabledMSAA()*/)
		{

			//D3D12_CLEAR_VALUE clearValue{ DXGI_FORMAT_R8G8B8A8_UNORM , { 0.0f, .0f, .0f, 1.0f } };
			auto desc = CD3DX12_RESOURCE_DESC1::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, static_cast<uint64_t>(event.GetWidth()), event.GetHeight());
			TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
				, D3D12_HEAP_FLAG_NONE
				, &desc
				, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
				, nullptr
				, nullptr
				, IID_PPV_ARGS(m_TextureMultiSampleResolved.ReleaseAndGetAddressOf()));


			if (m_SRVIndexScreenBuffer == -1)
			{
				m_SRVIndexScreenBuffer = m_DescHeapSRV->AddDescriptorSRV(m_TextureMultiSampleResolved.Get(), nullptr);
			}
			else
			{
				m_DescHeapSRV->ReplaceDescriptorSRV(m_TextureMultiSampleResolved.Get(), nullptr, m_SRVIndexScreenBuffer);
			}
		}
		else
		{
			auto dx12bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);
			m_D3D12Resource_ScreenBuffer = dx12bufferManager->GetResource(m_RenderTargetScreenBuffer);

			if (m_SRVIndexScreenBuffer == -1)
			{
				m_SRVIndexScreenBuffer = m_DescHeapSRV->AddDescriptorSRV(m_D3D12Resource_ScreenBuffer, nullptr);
			}
			else
			{
				m_DescHeapSRV->ReplaceDescriptorSRV(m_D3D12Resource_ScreenBuffer, nullptr, m_SRVIndexScreenBuffer);
			}
		}
	}

}

#endif
