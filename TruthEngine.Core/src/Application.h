#pragma once

#include "TruthEngine.Core.h"
#include "Core/Window.h"
#include "Core/Event/Event.h"

namespace TruthEngine::Core {

	class Application {

	public:
		Application(const char* title, uint32_t clientWidth, uint32_t clientHeight);
		virtual ~Application();


		uint32_t GetClientWidth() const noexcept;

		uint32_t GetClientHeight() const noexcept;

		const char* GetTitle() const noexcept;

		void* GetWindow() { return static_cast<void*>(m_Window.get()); }

		virtual void Run();
		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnProcess() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnEvent(Event& e);

		static Application& GetApplication();

	protected:

		static Application* s_Instance;

		std::unique_ptr<Window> m_Window;
		EventDispatcher m_EventDispatcher;

		uint32_t m_ClientWidth;
		uint32_t m_ClientHeight;

		std::string m_Title;

		bool m_Running = true;

	};

	//to be defined in client
	Application* CreateApplication();

}

#define TE_INSTANCE_APPLICATION TruthEngine::Core::Application::GetApplication()