#include "pch.h"
#include "Core/Application.h"

#include "Core/Renderer/GraphicDevice.h"
#include "core/Renderer/SwapChain.h"
#include "Core/Renderer/RendererLayer.h"

#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Light/LightManager.h"


#include "Core/Event/EventApplication.h"
#include "Core/Entity/Camera/CameraPerspective.h"
#include "Core/Entity/Camera/CameraManager.h"

#include "Core/Input/InputManager.h"

namespace TruthEngine {

	Application::Application(const char* title, uint32_t clientWidth, uint32_t clientHeight, uint8_t framesInFlightNum) : m_Title(title), m_ClientWidth(clientWidth), m_ClientHeight(clientHeight), m_FramesOnTheFlyNum(framesInFlightNum)
	{

		TE_ASSERT_CORE(!s_Instance, "Aplication already exists!");
		s_Instance = this;

		m_Window = TruthEngine::TECreateWindow(title, clientWidth, clientHeight);

		auto windowEventCallback = [this]
		(Event& event)
		{
			m_EventDispatcher.OnEvent(event);
		};

		m_Window->SetEventCallBack(windowEventCallback);

		auto listener_OnWindowResize = [this](Event& event)
		{
			OnWindowResize(static_cast<EventWindowResize&>(event));
		};
		RegisterEventListener(EventType::WindowResize, listener_OnWindowResize);

		m_RendererLayer = std::make_shared<RendererLayer>();
		
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

		TE_INSTANCE_THREADPOOL->Start(std::thread::hardware_concurrency());

		TE_RUN_TASK([]() { TE_LOG_CORE_INFO("This message is snet by threadID = {0}", std::this_thread::get_id()); });

		auto r = CreateGDevice(0);
		TE_ASSERT_CORE(TE_SUCCEEDED(r), "Creation of GDevice is failed!");

		m_Window->Show();

		m_EventDispatcher.RegisterListener(EventType::WindowClose, [this](const Event& e) { this->m_Running = false; });

		m_Timer.Reset();

		OnInit();

		while (m_Running)
		{
			m_Timer.Tick();

			OnUpdate();

			m_Window->OnUpdate();

			//m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_FramesInFlightNum;
		}
	}

	void Application::OnEvent(Event& e)
	{
		m_EventDispatcher.OnEvent(e);
	}

	void Application::OnWindowResize(EventWindowResize& event)
	{
		m_ClientWidth = event.GetWidth();
		m_ClientHeight = event.GetHeight();
	}

	uint8_t Application::GetCurrentFrameIndex() const noexcept
	{
		return TE_INSTANCE_SWAPCHAIN->GetCurrentFrameIndex(); 
	}

	Application* Application::s_Instance = nullptr;

}
