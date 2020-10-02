#pragma once


namespace TruthEngine::Core {

	class TruthEngineApp {

	public:
		TruthEngineApp(unsigned int clientWidth, unsigned int clientHeight);
		virtual ~TruthEngineApp();


		unsigned int GetClientWidth() const noexcept;
		unsigned int GetClientHeight() const noexcept;
		const char* GetTitle() const noexcept;

		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnProcess() = 0;
		virtual void OnDestroy() = 0;


		virtual void OnKeyDown(const unsigned char key) = 0;
		virtual void OnKeyUp(const unsigned char key) = 0;


	protected:

		unsigned int mClientWidth;
		unsigned int mClientHeight;

		std::string mTitle;

	};

}