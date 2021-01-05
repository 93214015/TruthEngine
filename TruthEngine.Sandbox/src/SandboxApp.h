#pragma once

#include "TruthEngine.h"


namespace TruthEngine::Sandbox {

	class SandboxApp : public TruthEngine::Core::Application {

	public:
		SandboxApp(uint16_t clientWidth, uint16_t clientHeight, uint8_t frameInFlightNum);


		void OnInit() override;


		void OnUpdate() override;


		void OnProcess() override;


		void OnDestroy() override;

	private:

	};

}

