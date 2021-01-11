#pragma once
#include "TruthEngine.h"

namespace TruthEngine
{
	namespace Editor
	{
		class ApplicationEditor : public Core::Application
		{
		public:
			ApplicationEditor(uint16_t clientWidth, uint16_t clientHeight, uint8_t framesInFlightNum);


			void OnInit() override;


			void OnUpdate() override;


			void OnProcess() override;


			void OnDestroy() override;

		private:

			void OnImGuiRender();

		};
	}
}