#pragma once


#ifdef TE_PLATFORM_WINDOWS

extern TruthEngine::Core::TruthEngineApp* TruthEngine::Core::CreateApplication();

#ifdef TE_WIN32

	int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_ LPWSTR    lpCmdLine,
		_In_ int       nCmdShow)
	{
		
		TruthEngine::Core::Log::Init();
		TE_LOG_CORE_WARN("Initialized Logger!");
		TE_LOG_APP_INFO("The App Started!");


		TruthEngine::Core::TruthEngineApp* app = TruthEngine::Core::CreateApplication();
		app->Run();
		delete app;
		
	}

#else

	int main(int argc, char** argv) {

		TruthEngine::Core::Log::Init();
		TE_LOG_CORE_WARN("Initialized Logger!");
		TE_LOG_APP_INFO("The App Started!");


		TruthEngine::Core::TruthEngineApp* app = TruthEngine::Core::CreateApplication();
		app->Run();
		delete app;
	}

#endif

#endif
