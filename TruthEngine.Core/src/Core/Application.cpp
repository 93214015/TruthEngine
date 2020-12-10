#include "pch.h"
#include "Core/Application.h"
#include "Core/Renderer/GraphicDevice.h"
#include "Core/Entity/Model/ModelManager.h"

namespace TruthEngine::Core {

	Application::Application(const char* title, uint32_t clientWidth, uint32_t clientHeight) : m_Title(title), m_ClientWidth(clientWidth), m_ClientHeight(clientHeight)
	{

		TE_ASSERT_CORE(!s_Instance, "Aplication already exists!");
		s_Instance = this;

		m_Window = TruthEngine::Core::TECreateWindow(title, clientWidth, clientHeight);

		m_Window->SetEventCallBack(std::bind(&EventDispatcher::OnEvent, &m_EventDispatcher, std::placeholders::_1));

		m_RendererLayer.reset(new RendererLayer());


	}

	Application::~Application() = default;


	void Application::Run()
	{

		TE_INSTANCE_THREADPOOL.Start(std::thread::hardware_concurrency());

		TE_RUN_TASK([]() { TE_LOG_CORE_INFO("This message is snet by threadID = {0}", std::this_thread::get_id()); });

		auto r = CreateGDevice(0);
		TE_ASSERT_CORE(r, "Creation of GDevice is failed!");

		m_Window->Show();

		m_EventDispatcher.RegisterListener(EventType::WindowClose, [this](const Event& e) { this->m_Running = false; });

		m_LayerStack.PushOverlay(m_RendererLayer.get());

		m_Timer.Reset();

		while (m_Running)
		{

			m_Timer.Tick();

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
