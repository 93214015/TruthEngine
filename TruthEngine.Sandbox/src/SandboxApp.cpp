#include "pch.h"
#include "SandboxApp.h"

namespace TruthEngine::Sandbox {

	void SandboxApp::OnInit()
	{

	}

	void SandboxApp::OnUpdate()
	{

	}

	void SandboxApp::OnProcess()
	{

	}

	void SandboxApp::OnDestroy()
	{

	}

	SandboxApp::SandboxApp(uint16_t clientWidth, uint16_t clientHeight) : Core::Application("SandBox", clientWidth, clientHeight)
	{

	}

}

TruthEngine::Core::Application* TruthEngine::Core::CreateApplication() {
	return new TruthEngine::Sandbox::SandboxApp(1024, 768);
}