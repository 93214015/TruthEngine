#pragma once

namespace TruthEngine
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
			void Panning(float right, float up);
			void Walk(float d);

			// Rotate the camera.
			void Pitch(float angle);
			void RotateY(float angle);
			void RotateCamera(float d);

			inline float GetSpeed()const
			{
				return m_Speed;
			}
			inline void SetSpeed(float speed)
			{
				m_Speed = speed;
			}

		protected:
			void OnMouseMove(EventMouseMoved& event);
			void OnKeyPressed(EventKeyPressed& event);
		protected:
			Camera* m_Camera = nullptr;

			float m_Speed = 10.0f;

		};
}
