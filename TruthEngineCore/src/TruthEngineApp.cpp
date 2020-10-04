#include "pch.h"
#include "TruthEngineApp.h"

namespace TruthEngine::Core {

	TruthEngineApp::TruthEngineApp(const char* title, uint32_t clientWidth, uint32_t clientHeight) : m_Title(title), m_ClientWidth(clientWidth), m_ClientHeight(clientHeight)
	{
		TE_ASSERT_CORE(!s_Instance, "Aplication already exists!");
		s_Instance = this;

		m_Window = TruthEngine::Core::CreateWindow();
	}

	TruthEngineApp::~TruthEngineApp() = default;

	uint32_t TruthEngineApp::GetClientWidth() const noexcept
	{
		return m_ClientWidth;
	}

	uint32_t TruthEngineApp::GetClientHeight() const noexcept
	{
		return m_ClientHeight;
	}

	const char* TruthEngineApp::GetTitle() const noexcept
	{
		return m_Title.c_str();
	}

	void TruthEngineApp::Run()
	{
		while (true);
	}

	TruthEngineApp* TruthEngineApp::GetApplication()
	{
		return s_Instance;
	}

	TruthEngineApp* TruthEngineApp::s_Instance = nullptr;

}
