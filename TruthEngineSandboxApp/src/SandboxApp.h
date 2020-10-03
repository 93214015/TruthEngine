#pragma once

#include "TruthEngine.h"


namespace TruthEngine::Sandbox {

	class SandboxApp : public TruthEngine::Core::TruthEngineApp {

	public:
		SandboxApp(uint32_t clientWidth, uint32_t clientHeight);

		void OnInit() override;


		void OnUpdate() override;


		void OnProcess() override;


		void OnDestroy() override;


		void OnKeyDown(const unsigned char key) override;


		void OnKeyUp(const unsigned char key) override;

	private:

	};

}

TruthEngine::Core::TruthEngineApp* TruthEngine::Core::CreateApplication() {
	return new TruthEngine::Sandbox::SandboxApp(0, 0);
}