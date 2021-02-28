#pragma once
#include "TruthEngine.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertyPanel.h"

#include "Core/PhysicEngine/PhysicsEngine.h"


namespace TruthEngine
{

	class EventKeyReleased;

	class ApplicationEditor : public Application
	{
	public:
		ApplicationEditor(uint16_t clientWidth, uint16_t clientHeight, uint8_t framesInFlightNum);


		void OnInit() override;


		void OnUpdate() override;


		void OnProcess() override;


		void OnDestroy() override;



	private:

		void OnImGuiRender();

		void RegisterOnEvents();

		void OnEventKeyReleased(EventKeyReleased& event);

	private:

		TimerProfile_OneSecond m_TimerAvg_Update;
		TimerProfile_OneSecond m_TimerAvg_ImGuiPass;
		TimerProfile_OneSecond m_TimerAvg_UpdateRenderPasses;

		float2 m_SceneViewPortPos = { .0f, .0f };
		float2 m_SceneViewPortAreaMin = { .0f, .0f };
		float2 m_SceneViewPortAreaMax = { .0f, .0f };

		SceneHierarchyPanel m_SceneHierarchyPanel;
		EntityPropertyPanel m_EntityPropertyPanel;

	};

}