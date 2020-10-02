#include "pch.h"
#include "TruthEngineApp.h"

namespace TruthEngine::Core {

	TruthEngineApp::TruthEngineApp(unsigned int clientWidth, unsigned int clientHeight) : mClientWidth(clientWidth), mClientHeight(clientHeight)
	{
	}

	TruthEngineApp::~TruthEngineApp() = default;

	unsigned int TruthEngineApp::GetClientWidth() const noexcept
	{
		return mClientWidth;
	}

	unsigned int TruthEngineApp::GetClientHeight() const noexcept
	{
		return mClientHeight;
	}

	const char* TruthEngineApp::GetTitle() const noexcept
	{
		return mTitle.c_str();
	}

}
