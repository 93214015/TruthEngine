#include "pch.h"
#include "Core/Application.h"
#include "Core/Renderer/GraphicDevice.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Light/LightManager.h"

#include "Core/Event/EventApplication.h"

#include "Core/Entity/Camera/CameraPerspective.h"
#include "Core/Entity/Camera/CameraManager.h"

#include "Core/Input/InputManager.h"

namespace TruthEngine::Core {

	Application::Application(const char* title, uint32_t clientWidth, uint32_t clientHeight) : m_Title(title), m_ClientWidth(clientWidth), m_ClientHeight(clientHeight)
	{

		TE_ASSERT_CORE(!s_Instance, "Aplication already exists!");
		s_Instance = this;

		m_Window = TruthEngine::Core::TECreateWindow(title, clientWidth, clientHeight);

		m_Window->SetEventCallBack(std::bind(&EventDispatcher::OnEvent, &m_EventDispatcher, std::placeholders::_1));

		m_RendererLayer.reset(new RendererLayer());

		auto mainCamera = std::make_shared<CameraPerspective>("mainCamera");
		mainCamera->SetPosition(0.0f, 0.0f, -10.0f); //3Cylinder_Big
		mainCamera->LookAt(mainCamera->GetPosition(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0, 1.0, 0.0));
		mainCamera->SetLens(DirectX::XM_PIDIV4, static_cast<float>(m_ClientWidth) / m_ClientHeight, 10.0f, 1000.0f);
		CameraManager::GetInstance()->AddCamera(mainCamera);

		InputManager::RegisterKey('A');
		InputManager::RegisterKey('W');
		InputManager::RegisterKey('S');
		InputManager::RegisterKey('D');
	}

	Application::~Application() = default;


	void Application::ResizeSceneViewport(uint32_t width, uint32_t height) noexcept
	{
		m_SceneViewportWidth = width;
		m_SceneViewportHeight = height;

		EventSceneViewportResize event{width, height};

		OnEvent(event);
	}

	void Application::Run()
	{

		TE_INSTANCE_THREADPOOL.Start(std::thread::hardware_concurrency());

		TE_RUN_TASK([]() { TE_LOG_CORE_INFO("This message is snet by threadID = {0}", std::this_thread::get_id()); });

		auto r = CreateGDevice(0);
		TE_ASSERT_CORE(r, "Creation of GDevice is failed!");

		m_Window->Show();

		m_EventDispatcher.RegisterListener(EventType::WindowClose, [this](const Event& e) { this->m_Running = false; });

		auto lightManager = LightManager::GetInstace();
		lightManager->AddLightDirectional("dlight_0", float4{ 0.7f, 0.7f, 0.7f, 0.7f }, float4{ 0.3f, 0.3f, 0.3f, 0.3f }, float4{ 0.0f, 0.0f, 0.0f, 0.0f }, float3{ 1.0f, -1.0f, 1.0f }, float3{ 0.0f, 20.0f, -20.0f }, 0.05f, false);

		m_LayerStack.PushOverlay(m_RendererLayer.get());

		m_Timer.Reset();

		//must put ModelManager initiation after RendererLayer attachment so that the bufferManager has been initiated 
		TE_INSTANCE_MODELMANAGER->Init(TE_INSTANCE_BUFFERMANAGER.get());
		TE_INSTANCE_MODELMANAGER->AddSampleModel();

		while (m_Running)
		{

			m_Timer.Tick();

			InputManager::ProcessInput();

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

			m_Window->OnUpdate();

			m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_FramesInFlightNum;
		}
	}

	void Application::OnEvent(Event& e)
	{
		m_EventDispatcher.OnEvent(e);
	}

	Application* Application::s_Instance = nullptr;

}
