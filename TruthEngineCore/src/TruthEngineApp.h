#pragma once

#include "TruthEngine.Core.h"
#include "Core/Window.h"

namespace TruthEngine::Core {

	class TruthEngineApp {

	public:
		TruthEngineApp(const char* title, uint32_t clientWidth, uint32_t clientHeight);
		virtual ~TruthEngineApp();


		uint32_t GetClientWidth() const noexcept;
		uint32_t GetClientHeight() const noexcept;
		const char* GetTitle() const noexcept;

		virtual void Run();
		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnProcess() = 0;
		virtual void OnDestroy() = 0;


		virtual void OnKeyDown(const unsigned char key) = 0;
		virtual void OnKeyUp(const unsigned char key) = 0;

		static TruthEngineApp* GetApplication();

	protected:

		static TruthEngineApp* s_Instance;

		std::unique_ptr<Window> m_Window;

		uint32_t m_ClientWidth;
		uint32_t m_ClientHeight;

		std::string m_Title;

	};

	//to be defined in client
	TruthEngineApp* CreateApplication();

}