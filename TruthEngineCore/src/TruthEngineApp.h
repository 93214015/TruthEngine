#pragma once

#include "TruthEngine.Core.h"

namespace TruthEngine::Core {

	class TRUTHENGINE_API TruthEngineApp {

	public:
		TruthEngineApp(uint32_t clientWidth, uint32_t clientHeight);
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


	protected:

		uint32_t mClientWidth;
		uint32_t mClientHeight;

		std::string mTitle;

	};

	//to be defined in client
	TruthEngineApp* CreateApplication();

}