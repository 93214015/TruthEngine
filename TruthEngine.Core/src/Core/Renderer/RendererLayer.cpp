#include "pch.h"
#include "RendererLayer.h"

#include "SwapChain.h"
#include "ConstantBuffer.h"

#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Entity/Model/ModelManager.h"



namespace TruthEngine::Core
{

	RendererLayer::RendererLayer() : m_ImGuiLayer(ImGuiLayer::Factory()), m_Renderer3D(std::make_shared<RenderPass_ForwardRendering>())
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


		m_RendererCommand.Init(TE_IDX_RENDERPASS::NONE, TE_IDX_SHADERCLASS::NONE);

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

		m_CB_PerFrame = m_BufferManager->CreateConstantBufferUpload<ConstantBuffer_Data_Per_Frame>(TE_IDX_CONSTANTBUFFER::PER_FRAME);

		m_CB_PerFrame->GetData()->m_Color = DirectX::XMFLOAT4{ 0.2f, 0.7f, 0.4f, 1.0f };
	}

	void RendererLayer::OnDetach()
	{
		m_ImGuiLayer->OnDetach();
	}

	void RendererLayer::OnUpdate(double deltaFrameTime)
	{
		auto color = m_CB_PerFrame->GetData()->m_Color;
		color.x = std::fmod(color.x + 0.01f, 1.0f);
		color.y = std::fmod(color.y + 0.01f, 1.0f);
		color.z = std::fmod(color.z + 0.01f, 1.0f);
		color.w = std::fmod(color.w + 0.01f, 1.0f);
		m_CB_PerFrame->GetData()->m_Color = color;

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

