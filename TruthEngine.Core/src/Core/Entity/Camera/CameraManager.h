#pragma once

#include "Camera.h"
#include "CameraController.h"

namespace TruthEngine
{

	namespace Core
	{

		class CameraManager
		{
		public:
			/*void AddCamera(std::shared_ptr<ICamera> camera);*/

			Camera* GetCamera(std::string_view name);
			CameraController* GetCameraController();

			Camera* CreatePerspectiveFOV(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane);
			Camera* CreatePerspective(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane);
			Camera* CreatePerspectiveCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographicCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographic(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane);

			static CameraManager* GetInstance();

		protected:


		protected:
			uint32_t CameraNum = 0;

			std::unordered_map<std::string_view, Camera> m_Map_Cameras;
			
			CameraController m_CameraController;

		};
	}
}
