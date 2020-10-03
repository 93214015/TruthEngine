#pragma once


#ifdef TE_PLATFORM_WINDOWS

extern TruthEngine::Core::TruthEngineApp* TruthEngine::Core::CreateApplication();

int main(int argc, char** argv) {

	TruthEngine::Core::Log::Init();
	TE_LOG_CORE_WARN("Initialized Logger!");
	TE_LOG_APP_INFO("The App Started!");


	TruthEngine::Core::TruthEngineApp* app = TruthEngine::Core::CreateApplication();
	app->Run();
	delete app;
}

#endif
