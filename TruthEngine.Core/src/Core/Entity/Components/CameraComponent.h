#pragma once
#include "Core/Entity/Camera/Camera.h"

namespace TruthEngine
{
	class CameraComponent
	{
	public:
		CameraComponent(Camera* camera)
			: m_Camera(camera), m_Controller(nullptr)
		{}
		CameraComponent(Camera* camera, CameraController* controller)
			: m_Camera(camera), m_Controller(controller)
		{}

		inline Camera* GetCamera()
		{
			return m_Camera;
		}
		inline CameraController* GetCameraController()
		{
			return m_Controller;
		}
		inline bool HasCameraController()
		{
			return m_Controller != nullptr;
		}

	private:
		Camera* m_Camera;
		CameraController* m_Controller;
	};
}
