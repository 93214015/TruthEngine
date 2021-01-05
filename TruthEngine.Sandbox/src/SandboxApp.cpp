#include "pch.h"
#include "SandboxApp.h"

#include "Core/Input/InputManager.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Light/LightManager.h"

namespace TruthEngine::Sandbox {

	void SandboxApp::OnInit()
	{
		auto lightManager = Core::LightManager::GetInstace();
		lightManager->AddLightDirectional("dlight_0", float4{ 0.8f, 0.8f, 0.8f, 0.8f }, float4{ 0.3f, 0.3f, 0.3f, 0.3f }, float4{ 0.0f, 0.0f, 0.0f, 0.0f }, float3{ 1.0f, -1.0f, 1.0f }, float3{ 0.0f, 20.0f, -20.0f }, 0.05f, false);

		m_LayerStack.PushLayer(m_RendererLayer.get());

		//must put ModelManager initiation after RendererLayer attachment so that the bufferManager has been initiated 
		auto modelManager = Core::ModelManager::GetInstance().get();
		modelManager->Init(TE_INSTANCE_BUFFERMANAGER.get());
		modelManager->AddSampleModel();
	}

	void SandboxApp::OnUpdate()
	{
		Core::InputManager::ProcessInput();

		m_RendererLayer->BeginRendering();

		for (auto layer : m_LayerStack)
		{
			layer->OnUpdate(m_Timer.DeltaTime());
		}

		if (m_RendererLayer->BeginImGuiLayer())
		{
			for (auto layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_RendererLayer->EndImGuiLayer();
		}

		m_RendererLayer->EndRendering();
	}

	void SandboxApp::OnProcess()
	{

	}

	void SandboxApp::OnDestroy()
	{
		
	}

	SandboxApp::SandboxApp(uint16_t clientWidth, uint16_t clientHeight, uint8_t frameInFlightNum) : Core::Application("SandBox", clientWidth, clientHeight, frameInFlightNum)
	{

	}

}

TruthEngine::Core::Application* TruthEngine::Core::CreateApplication() {
	return new TruthEngine::Sandbox::SandboxApp(1024, 768, 2);
}