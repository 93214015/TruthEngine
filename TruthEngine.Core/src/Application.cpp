#include "pch.h"
#include "Application.h"

namespace TruthEngine::Core {

	Application::Application(const char* title, uint32_t clientWidth, uint32_t clientHeight) : m_Title(title), m_ClientWidth(clientWidth), m_ClientHeight(clientHeight)
	{
		TE_ASSERT_CORE(!s_Instance, "Aplication already exists!");
		s_Instance = this;

		m_Window = TruthEngine::Core::TECreateWindow(title, clientWidth, clientHeight);

		m_Window->SetEventCallBack(std::bind(&EventDispatcher::OnEvent, &m_EventDispatcher, std::placeholders::_1));

	}

	Application::~Application() = default;


	void Application::Run()
	{

		TE_INSTANCE_THREADPOOL.Start(std::thread::hardware_concurrency());

		TE_RUN_TASK([]() { TE_LOG_CORE_INFO("This message is snet by threadID = {0}", std::this_thread::get_id()); });

		m_Window->Show();

		m_EventDispatcher.RegisterListener(EventType::WindowClose, [this](const Event& e) { this->m_Running = false; });

		while (m_Running) 
		{

			m_Window->OnUpdate();

		}
	}

	void Application::OnEvent(Event& e)
	{
		m_EventDispatcher.OnEvent(e);
	}

	Application* Application::s_Instance = nullptr;

}
