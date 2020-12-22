#pragma once

#include "Core/Window.h"
#include "Core/Event/Event.h"
#include "Core/LayerStack.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Renderer/RendererLayer.h"
#include "Core/TimerEngine.h"

namespace TruthEngine::Core {

	class Application {

	public:
		Application(const char* title, uint32_t clientWidth, uint32_t clientHeight);
		virtual ~Application();


		inline uint32_t GetClientWidth() const noexcept { return m_ClientWidth; }

		inline uint32_t GetClientHeight() const noexcept { return m_ClientHeight; }

		inline uint32_t GetSceneViewportWidth() const noexcept { return m_SceneViewportWidth; }

		inline uint32_t GetSceneViewportHeight() const noexcept { return m_SceneViewportHeight; }


		inline uint32_t GetFramesInFlightNum() const noexcept { return m_FramesInFlightNum; }

		inline uint32_t GetCurrentFrameIndex() const noexcept { return m_CurrentFrameIndex; }

		inline const char* GetTitle() const noexcept { return m_Title.c_str(); }

		inline void RegisterEventListener(EventType eventType, const EventListener& listener)
		{
			m_EventDispatcher.RegisterListener(eventType, listener);
		}

		void ResizeSceneViewport(uint32_t width, uint32_t height) noexcept;

		Window* GetWindow() { return m_Window.get(); }

		virtual void Run();
		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnProcess() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnEvent(Event& e);


		static inline Application* GetApplication() { return s_Instance; }

	protected:

		static Application* s_Instance;

		std::unique_ptr<Window> m_Window;
		EventDispatcher m_EventDispatcher;

		LayerStack m_LayerStack;

		std::shared_ptr<RendererLayer> m_RendererLayer;


		TimerEngine m_Timer;


		uint32_t m_ClientWidth = 0;
		uint32_t m_ClientHeight = 0;
		uint32_t m_SceneViewportWidth = 0;
		uint32_t m_SceneViewportHeight = 0;
		uint32_t m_FramesInFlightNum = 2;
		uint32_t m_CurrentFrameIndex = 0;

		std::string m_Title;

		bool m_Running = true;

	};

	//to be defined in client
	Application* CreateApplication();

}

#define TE_INSTANCE_APPLICATION TruthEngine::Core::Application::GetApplication()