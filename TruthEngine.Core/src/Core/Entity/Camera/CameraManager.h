#pragma once

#include "Camera.h"
#include "CameraController.h"

namespace TruthEngine
{

	namespace Core
	{

		class EventSceneViewportResize;

		class CameraManager
		{
		public:
			CameraManager();
			/*void AddCamera(std::shared_ptr<ICamera> camera);*/

			Camera* GetCamera(std::string_view name);
			CameraController* GetCameraController();

			Camera* CreatePerspectiveFOV(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane);
			Camera* CreatePerspectiveDirection(const char* name, const float3& position, const float3& look, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane);
			Camera* CreatePerspectiveTarget(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane);
			Camera* CreatePerspectiveCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographicCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographicTarget(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographicDirection(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);

			inline Camera* GetActiveCamera()
			{
				return m_ActiveCamera;
			}

			TE_RESULT SetActiveCamera(const char* cameraName);
			TE_RESULT SetActiveCamera(Camera* camera);

			void OnResizeViewport(const EventSceneViewportResize& event);

			void EditCameraFrustum(Camera* camera);
			void EditCameraFrustumPerspective(Camera* camera, float aspectRatio, float zNearPlane = -1.0f, float zFarPlane = -1.0f);
			void EditCameraFrustumPerspective(Camera* camera);
			void EditCameraFrustumOrthographic(Camera* camera, float aspectRatio, float zNearPlane = -1.0, float zFarPlane = -1.0f);
			void EditCameraFrustumOrthographic(Camera* camera);

			inline std::unordered_map<uint32_t, Camera>::iterator begin()
			{
				return m_Map_Cameras.begin();
			}
			inline std::unordered_map<uint32_t, Camera>::const_iterator cbegin() const
			{
				return m_Map_Cameras.cbegin();
			}
			inline std::unordered_map<uint32_t, Camera>::iterator end()
			{
				return m_Map_Cameras.end();
			}
			inline std::unordered_map<uint32_t, Camera>::const_iterator cend()
			{
				return m_Map_Cameras.cend();
			}

			static CameraManager* GetInstance();


		protected:


		protected:
			std::unordered_map<uint32_t, Camera> m_Map_Cameras;
			std::unordered_map<std::string_view, Camera*> m_Map_CamerasName;
			
			Camera* m_ActiveCamera;

			CameraController m_CameraController;

		};
	}
}
