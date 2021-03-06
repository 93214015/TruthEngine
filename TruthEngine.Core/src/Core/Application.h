#pragma once

#include "Core/Window.h"
#include "Core/Event/EventDispatcher.h"
#include "Core/LayerStack.h"
#include "Core/TimerEngine.h"

#include "Core/Entity/Camera/CameraPerspective.h"

namespace TruthEngine {

	class EventWindowResize;
	class RendererLayer;
	class Scene;

	class Application {

	public:
		Application(const char* title, uint32_t clientWidth, uint32_t clientHeight, uint8_t framesInFlightNum);
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;


		inline uint32_t GetClientWidth() const noexcept { return m_ClientWidth; }

		inline uint32_t GetClientHeight() const noexcept { return m_ClientHeight; }

		inline uint32_t GetSceneViewportWidth() const noexcept { return m_SceneViewportWidth; }

		inline uint32_t GetSceneViewportHeight() const noexcept { return m_SceneViewportHeight; }

		inline float GetSceneViewportAspectRatio() const noexcept { return static_cast<float>(m_SceneViewportWidth)/ m_SceneViewportHeight; };

		inline uint8_t GetFramesOnTheFlyNum() const noexcept { return m_FramesOnTheFlyNum; }

		uint8_t GetCurrentFrameIndex() const noexcept;
		//inline uint32_t GetCurrentFrameIndex() const noexcept { return m_CurrentFrameIndex; }

		inline const char* GetTitle() const noexcept { return m_Title.c_str(); }


		EventListenerHandle RegisterEventListener(EventType eventType, const EventListener& listener)
		{
			return m_EventDispatcher.RegisterListener(eventType, listener);
		}

		void UnRegisterEventListener(EventListenerHandle* _EventListenerHandles, size_t _ListenerCount)
		{
			m_EventDispatcher.UnRegisterListener(_EventListenerHandles, _ListenerCount);
		}


		inline double GetAverageCPUTime()const noexcept { return m_Timer.GetAverageCpuTime(); }
		inline double FrameTime() const noexcept { return m_Timer.DeltaTime(); }
		inline uint32_t GetFPS()const noexcept { return m_Timer.GetFPS(); }
		inline bool GetWindowMode() const noexcept { return m_WindowMode; }
		inline void SetWindowMode(bool windowedMode) noexcept { m_WindowMode = windowedMode; }

		void ResizeSceneViewport(uint32_t width, uint32_t height) noexcept;

		Window* GetWindow() { return m_Window.get(); }

		virtual void Run();
		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnProcess() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnEvent(Event& e);

		inline bool IsHoveredSceneViewPort() const noexcept { return m_IsHoveredSceneViewport; }
		inline void IsHoveredSceneViewPort(bool hovered) noexcept { m_IsHoveredSceneViewport = hovered; }

		RendererLayer* GetRendererLayer();

		static Application* GetApplication() { return s_Instance; }		

	protected:
		void OnWindowResize(EventWindowResize& event);


	protected:
		static Application* s_Instance;

		std::unique_ptr<Scene> m_DefautlScene;
		
		std::unique_ptr<Window> m_Window;
		EventDispatcher m_EventDispatcher;

		LayerStack m_LayerStack;

		std::shared_ptr<RendererLayer> m_RendererLayer;

		TimerEngine m_Timer;


		uint32_t m_ClientWidth;
		uint32_t m_ClientHeight;
		uint32_t m_SceneViewportWidth = 1280;
		uint32_t m_SceneViewportHeight = 720;

		uint8_t m_CurrentFrameOnTheFly = 0;
		uint8_t m_FramesOnTheFlyNum = 3;

		std::string m_Title;

		bool m_IsRenderingFrame = true;
		bool m_Running = true;
		bool m_WindowMode = true;
		bool m_IsHoveredSceneViewport = false;

	};

	//to be defined in the client
	Application* CreateApplication();

}

#define TE_INSTANCE_APPLICATION TruthEngine::Application::GetApplication()
