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

	uint32_t Application::GetClientWidth() const noexcept
	{
		return m_ClientWidth;
	}

	uint32_t Application::GetClientHeight() const noexcept
	{
		return m_ClientHeight;
	}

	const char* Application::GetTitle() const noexcept
	{
		return m_Title.c_str();
	}

	void Application::Run()
	{
		m_Window->Show();

		m_EventDispatcher.RegisterListener(EventType::WindowClose, [this](const Event& e) { this->m_Running = false; });

		while (m_Running) 
		{

			m_Window->OnUpdate();

		}
	}

	void Application::OnEvent(const Event& e)
	{
		m_EventDispatcher.OnEvent(e);
	}

	Application* Application::GetApplication()
	{
		return s_Instance;
	}

	Application* Application::s_Instance = nullptr;

}
