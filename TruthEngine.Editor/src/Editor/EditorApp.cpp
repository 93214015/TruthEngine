#include "pch.h"
#include "EditorApp.h"

#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Input/InputManager.h"
#include "Core/Entity/Camera/CameraManager.h"




namespace TruthEngine::Editor
{


	ApplicationEditor::ApplicationEditor(uint16_t clientWidth, uint16_t clientHeight, uint8_t framesInFlightNum)
		: Core::Application("TruthEngine.Editor", clientWidth, clientHeight, framesInFlightNum)
	{
	}


	void ApplicationEditor::OnInit()
	{


		Core::InputManager::RegisterKey('A');
		Core::InputManager::RegisterKey('W');
		Core::InputManager::RegisterKey('S');
		Core::InputManager::RegisterKey('D');


		/*auto mainCamera = std::make_shared<Core::CameraPerspective>("mainCamera");
		mainCamera->SetPosition(0.0f, 0.0f, -10.0f); //3Cylinder_Big
		mainCamera->LookAt(mainCamera->GetPosition(), float3(0.0f, 0.0f, 0.0f), float3(0.0, 1.0, 0.0));
		mainCamera->SetLens(DirectX::XM_PIDIV4, static_cast<float>(m_ClientWidth) / m_ClientHeight, 10.0f, 1000.0f);
		mainCamera->SetSpeed(0.1f);
		Core::CameraManager::GetInstance()->AddCamera(mainCamera);*/

		auto mainCamera = Core::CameraManager::GetInstance()->CreatePerspectiveFOV("mainCamera"
			, float3{ 0.0f, 0.0f, -50.0f }
			, float3{ 0.0f, 0.0f, 0.0f }
			, float3{ 0.0f, 1.0f, 0.0f }
			, DirectX::XM_PIDIV4
			, static_cast<float>(m_ClientWidth) / m_ClientHeight
			, 10.0f
			, 1000.0f
		);

		Core::CameraManager::GetInstance()->GetCameraController()->AttachCamera(mainCamera);

		auto lightManager = Core::LightManager::GetInstace();
		lightManager->AddLightDirectional("dlight_0", float4{ 0.8f, 0.8f, 0.8f, 0.8f }, float4{ 0.3f, 0.3f, 0.3f, 0.3f }, float4{ 0.0f, 0.0f, 0.0f, 0.0f }, float3{ 1.0f, -1.0f, 1.0f }, float3{ 0.0f, 20.0f, -20.0f }, 0.05f, false);

		m_LayerStack.PushLayer(m_RendererLayer.get());

		//must put ModelManager initiation after RendererLayer attachment so that the bufferManager has been initiated 
		auto modelManager = Core::ModelManager::GetInstance().get();
		modelManager->Init(TE_INSTANCE_BUFFERMANAGER.get());
		//modelManager->AddSampleModel();
	}


	void ApplicationEditor::OnUpdate()
	{
		Core::InputManager::ProcessInput();

		m_RendererLayer->BeginRendering();

		for (auto layer : m_LayerStack)
		{
			layer->OnUpdate(m_Timer.DeltaTime());
		}

		if (m_RendererLayer->BeginImGuiLayer())
		{
			OnImGuiRender();
			for (auto layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_RendererLayer->EndImGuiLayer();
		}

		m_RendererLayer->EndRendering();
	}


	void ApplicationEditor::OnProcess()
	{
	}


	void ApplicationEditor::OnDestroy()
	{
	}

	void ApplicationEditor::OnImGuiRender()
	{
		ImGui::Begin("Settings");

		ImGui::End();
	}


}

TruthEngine::Core::Application* TruthEngine::Core::CreateApplication() {
	return new TruthEngine::Editor::ApplicationEditor(1024, 768, 2);
}