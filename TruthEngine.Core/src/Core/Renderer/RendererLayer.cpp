#include "pch.h"
#include "RendererLayer.h"

#include "SwapChain.h"
#include "ConstantBuffer.h"


#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Event/EventApplication.h"
#include "Core/Event/EventEntity.h"


#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Entity/Camera/CameraPerspective.h"

#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Light/LightDirectional.h"

namespace TruthEngine::Core
{

	RendererLayer::RendererLayer() : m_ImGuiLayer(ImGuiLayer::Factory()), m_RenderPass_ForwardRendering(std::make_shared<RenderPass_ForwardRendering>())
	{
	}
	RendererLayer::~RendererLayer() = default;

	RendererLayer::RendererLayer(const RendererLayer& renderer) = default;
	RendererLayer& RendererLayer::operator=(const RendererLayer& renderer) = default;


	void RendererLayer::OnAttach()
	{
		m_BufferManager = TE_INSTANCE_BUFFERMANAGER;

		m_BufferManager->Init(100, 1000, 10, 10);

		// init singleton object of dx12 swap chain
		TE_INSTANCE_SWAPCHAIN->Init(TE_INSTANCE_APPLICATION->GetClientWidth(), TE_INSTANCE_APPLICATION->GetClientHeight(), TE_INSTANCE_APPLICATION->GetWindow(), TE_INSTANCE_APPLICATION->GetFramesInFlightNum());

		m_RendererCommand.Init(TE_IDX_RENDERPASS::NONE, TE_IDX_SHADERCLASS::NONE);

		m_ModelManagers = TE_INSTANCE_MODELMANAGER;

		m_RendererCommand.CreateRenderTarget(TE_IDX_TEXTURE::RT_SCENEBUFFER, TE_INSTANCE_APPLICATION->GetClientWidth(), TE_INSTANCE_APPLICATION->GetClientHeight(), TE_RESOURCE_FORMAT::R8G8B8A8_UNORM, ClearValue_RenderTarget{ 1.0f, 1.0f, 1.0f, 1.0f }, true);

		m_RendererCommand.CreateRenderTargetView(TE_INSTANCE_SWAPCHAIN, &m_RTVBackBuffer);

		m_CB_PerFrame = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_Per_Frame>(TE_IDX_CONSTANTBUFFER::PER_FRAME);
		m_CB_PerDLight = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_Per_DLight>(TE_IDX_CONSTANTBUFFER::PER_DLIGHT);
		m_CB_Materials = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_Materials>(TE_IDX_CONSTANTBUFFER::MATERIALS);
		

		const auto& lightdata = LightManager::GetInstace()->GetDirectionalLight("dlight_0")->GetDirectionalLightData();
		*(m_CB_PerDLight->GetData()) = static_cast<const ConstantBuffer_Data_Per_DLight&>(lightdata);

		m_ImGuiLayer->OnAttach();

		m_RenderPassStack.PushRenderPass(m_RenderPass_ForwardRendering.get());

		RegisterEvents();
	}

	void RendererLayer::OnDetach()
	{
		m_RenderPassStack.PopAll();

		m_ImGuiLayer->OnDetach();

		TE_INSTANCE_SWAPCHAIN->Release();

		m_BufferManager->Release();
	}

	void RendererLayer::OnUpdate(double deltaFrameTime)
	{

		auto data_perFrame = m_CB_PerFrame->GetData();


		static auto mainCamera = CameraManager::GetInstance()->GetCamera("mainCamera");

		data_perFrame->EyePos = mainCamera->GetPosition();
		data_perFrame->ViewProj = mainCamera->GetViewProj();

		/*m_Model3DQueue.clear();
		for (auto& model : m_ModelManagers->GetModel3D())
		{
			m_Model3DQueue.emplace_back(model.get());
		}*/

		m_RenderPass_ForwardRendering->BeginScene();
		m_RenderPass_ForwardRendering->EndScene();
		m_RenderPass_ForwardRendering->Render();

		for (auto renderPass : m_RenderPassStack)
		{
		}
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

	void RendererLayer::RegisterEvents()
	{
		auto listener_windowResize = [this](Event& event) {  OnWindowResize(static_cast<EventWindowResize&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::WindowResize, listener_windowResize);

		auto listener_sceneViewportResize = [this](Event& event) {  OnSceneViewportResize(static_cast<EventSceneViewportResize&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::SceneViewportResize, listener_sceneViewportResize);

		auto listener_addMaterial = [this](Event& event) { OnAddMaterial(static_cast<EventEntityAddMaterial&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityAddMaterial, listener_addMaterial);
	}

	void RendererLayer::OnWindowResize(const EventWindowResize& event)
	{
		auto width = event.GetWidth();
		auto height = event.GetHeight();

		m_RendererCommand.ResizeSwapChain(TE_INSTANCE_SWAPCHAIN, width, height, &m_RTVBackBuffer, nullptr);

	}

	void RendererLayer::OnSceneViewportResize(const EventSceneViewportResize& event)
	{
		auto width = event.GetWidth();
		auto height = event.GetHeight();

		m_RendererCommand.ResizeRenderTarget(TE_IDX_TEXTURE::RT_SCENEBUFFER, width, height, nullptr, nullptr);

		m_RenderPass_ForwardRendering->OnSceneViewportResize(width, height);

	}

	void RendererLayer::OnAddMaterial(const EventEntityAddMaterial& event)
	{
		auto material = event.GetMaterial();
		auto& cbMaterialData = m_CB_Materials->GetData()->MaterialArray[material->GetID()];
		cbMaterialData = ConstantBuffer_Data_Materials::Material(material->GetColorDiffuse(), material->GetFresnelR0(), material->GetShininess(), material->GetMapIndexDiffuse(), material->GetMapIndexNormal(), material->GetMapIndexDisplacement());

	}

}

