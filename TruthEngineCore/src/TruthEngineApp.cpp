#include "pch.h"
#include "TruthEngineApp.h"

namespace TruthEngine::Core {

	TruthEngineApp::TruthEngineApp(uint32_t clientWidth, uint32_t clientHeight) : mClientWidth(clientWidth), mClientHeight(clientHeight)
	{
	}

	TruthEngineApp::~TruthEngineApp() = default;

	uint32_t TruthEngineApp::GetClientWidth() const noexcept
	{
		return mClientWidth;
	}

	uint32_t TruthEngineApp::GetClientHeight() const noexcept
	{
		return mClientHeight;
	}

	const char* TruthEngineApp::GetTitle() const noexcept
	{
		return mTitle.c_str();
	}

	void TruthEngineApp::Run()
	{
		while (true);
	}

}
