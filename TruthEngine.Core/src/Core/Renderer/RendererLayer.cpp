#include "pch.h"
#include "RendererLayer.h"

#include "SwapChain.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Entity/Model/ModelManager.h"


namespace TruthEngine::Core
{

	RendererLayer::RendererLayer() : m_ImGuiLayer(ImGuiLayer::Factory()), m_Renderer3D(std::make_shared<Renderer3D>())
	{
	}
	RendererLayer::~RendererLayer() = default;

	RendererLayer::RendererLayer(const RendererLayer& renderer) = default;
	RendererLayer& RendererLayer::operator=(const RendererLayer& renderer) = default;


	void RendererLayer::OnAttach()
	{
		m_BufferManager = TE_INSTANCE_BUFFERMANAGER;

		m_BufferManager->Init(20, 10, 10, 10);


		m_ImGuiLayer->OnAttach();


		m_RendererCommand.Init();

		m_RendererCommand.Begin();

		m_ModelManagers = TE_INSTANCE_MODELMANAGER;
		m_ModelManagers->Init(m_BufferManager.get(), &m_RendererCommand);

		m_RendererCommand.End();

	    m_Renderer3D->Init(m_BufferManager.get(), m_ModelManagers->GetMaterials());

		for (auto& model : m_ModelManagers->GetModel3D())
		{
			m_Model3DQueue.emplace_back(&model);
		}

		m_RTVBackBuffer = m_BufferManager->CreateRenderTargetView(TE_INSTANCE_SWAPCHAIN);

	}

	void RendererLayer::OnDetach()
	{
		m_ImGuiLayer->OnDetach();
	}

	void RendererLayer::OnUpdate(double deltaFrameTime)
	{
		m_Renderer3D->BeginScene();
		m_Renderer3D->EndScene();
		m_Renderer3D->Render(m_Model3DQueue);
	}


	bool RendererLayer::BeginImGuiLayer()
	{
		if (m_EnabledImGuiLayer)
		{
			m_ImGuiLayer->Begin();
		}
		return m_EnabledImGuiLayer;
	}

	void RendererLayer::EndImGuiLayer()
	{
		if (m_EnabledImGuiLayer)
		{
			m_ImGuiLayer->End();
		}
	}

	void RendererLayer::BeginRendering()
	{
		auto swapchain = TE_INSTANCE_SWAPCHAIN;
		m_RendererCommand.Begin();
		m_RendererCommand.SetRenderTarget(swapchain, m_RTVBackBuffer);
		m_RendererCommand.ClearRenderTarget(swapchain, m_RTVBackBuffer);
		m_RendererCommand.End();
	}

	void RendererLayer::EndRendering()
	{
		m_RendererCommand.Begin();
		m_RendererCommand.EndAndPresent();
	}

}

