#pragma once
#include "TruthEngine.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertyPanel.h"

#include "Core/PhysicEngine/PhysicsEngine.h"


namespace TruthEngine
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


		private:

			SceneHierarchyPanel m_SceneHierarchyPanel;
			EntityPropertyPanel m_EntityPropertyPanel;

		};

}