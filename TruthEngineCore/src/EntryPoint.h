#pragma once

#include "TruthEngineApp.h"

#ifdef TE_PLATFORM_WINDOWS

extern TruthEngine::Core::TruthEngineApp* TruthEngine::Core::CreateApplication();

int main(int argc, char** argv) {
	TruthEngine::Core::TruthEngineApp* app = TruthEngine::Core::CreateApplication();
	app->Run();
	delete app;
}

#endif
