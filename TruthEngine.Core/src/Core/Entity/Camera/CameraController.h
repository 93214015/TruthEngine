#pragma once

namespace TruthEngine
{
	namespace Core
	{
		class Camera;
		class EventMouseMoved;
		class EventKeyPressed;

		class CameraController
		{
		public:

			CameraController();

			inline void AttachCamera(Camera* camera)
			{
				m_Camera = camera;
			}

			inline Camera* GetCamera()
			{
				return m_Camera;
			}

			// Strafe/Walk the camera a distance d.
			void Strafe(float d);
			void Walk(float d);

			// Rotate the camera.
			void Pitch(float angle);
			void RotateY(float angle);
			void RotateCamera(float d);

		protected:
			void OnMouseMove(EventMouseMoved& event);
			void OnKeyPressed(EventKeyPressed& event);
		protected:
			Camera* m_Camera = nullptr;

			float m_Speed = 1.0f;

		};
	}
}
