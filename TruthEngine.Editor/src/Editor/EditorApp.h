#pragma once
#include "TruthEngine.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertyPanel.h"

#include "Core/PhysicEngine/PhysicsEngine.h"


namespace TruthEngine
{

	namespace Core
	{
		class EventKeyReleased;
	}

	class ApplicationEditor : public Core::Application
	{
	public:
		ApplicationEditor(uint16_t clientWidth, uint16_t clientHeight, uint8_t framesInFlightNum);


		void OnInit() override;


		void OnUpdate() override;


		void OnProcess() override;


		void OnDestroy() override;


		void OnEventKeyReleased(Core::EventKeyReleased& event);

	private:

		void OnImGuiRender();


	private:

		Core::TimerProfile_OneSecond m_TimerAvg_Update;
		Core::TimerProfile_OneSecond m_TimerAvg_ImGuiPass;
		Core::TimerProfile_OneSecond m_TimerAvg_UpdateRenderPasses;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		EntityPropertyPanel m_EntityPropertyPanel;

	};

}