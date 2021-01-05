#include "pch.h"
#include "CameraManager.h"

#include "CameraPerspective.h"
#include "CameraOrthographic.h"

namespace TruthEngine {

	namespace Core
	{

		

		void CameraManager::AddCamera(std::shared_ptr<ICamera> camera)
		{
			m_Map_Cameras[camera->m_Name] = camera;

			if (m_ActiveCamera == nullptr)
			{
				m_ActiveCamera = camera;
				camera->Active();
			}
		}

		std::shared_ptr<ICamera> CameraManager::GetCamera(std::string_view name)
		{
			return m_Map_Cameras[name];
		}

		void CameraManager::SetActiveCamera(std::string_view name)
		{
			if (m_ActiveCamera != nullptr)
			{
				m_ActiveCamera->Deactive();
			}

			m_ActiveCamera = m_Map_Cameras[name];
			m_ActiveCamera->Active();
		}

		CameraManager* CameraManager::GetInstance()
		{
			static CameraManager s_Instance;
			return &s_Instance;
		}

	}
}