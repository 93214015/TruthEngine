#pragma once

#include "Camera.h"
#include "CameraCascadedFrustum.h"
#include "CameraController.h"

namespace TruthEngine
{
		class EventSceneViewportResize;

		class CameraManager
		{
		public:
			CameraManager();
			/*void AddCamera(std::shared_ptr<ICamera> camera);*/

			Camera* GetCamera(std::string_view name);
			CameraCascadedFrustumBase* GetCameraCascaded(std::string_view name);
			CameraController* GetCameraController();

			Camera* CreatePerspectiveFOV(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane);
			Camera* CreatePerspectiveDirection(const char* name, const float3& position, const float3& look, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane);
			Camera* CreatePerspectiveTarget(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane);
			Camera* CreatePerspectiveCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographicCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographicTarget(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane);
			Camera* CreateOrthographicDirection(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane);

			template<uint32_t cascadeNum>
			CameraCascadedFrustum<cascadeNum>* CreateOrthographicCascaded(const char* name, const float cascadeDepths[cascadeNum], const float3& position, const float3& direction, const float3& worldUpVector);

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
			std::unordered_map<uint32_t, std::shared_ptr<CameraCascadedFrustumBase>> m_Map_CameraCascaded;
			std::unordered_map<std::string_view, CameraCascadedFrustumBase*> m_Map_CameraCascadedName;
			
			Camera* m_ActiveCamera;

			CameraController m_CameraController;

		};


		template<uint32_t cascadeNum>
		CameraCascadedFrustum<cascadeNum>* TruthEngine::CameraManager::CreateOrthographicCascaded(const char* name, const float cascadeDepths[cascadeNum], const float3& position, const float3& direction, const float3& worldUpVector)
		{
			using namespace DirectX;

			auto vPosition	= XMLoadFloat3(&position);
			auto vWorldUp	= XMLoadFloat3(&worldUpVector);

			XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&direction));
			XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
			XMVECTOR U = XMVector3Cross(L, R);

			float3 look, right, up;
			XMStoreFloat3(&look, L);
			XMStoreFloat3(&right, R);
			XMStoreFloat3(&up, U);

			CameraCascadedFrustum<cascadeNum>* camera = static_cast<CameraCascadedFrustum<cascadeNum>*>(GetCameraCascaded(name));
			if (camera) // if camera with the same name was existed then we recreate that in the place
			{
				return camera;
			}
			else
			{
				uint32_t cameraID = m_Map_CameraCascaded.size();
				auto cameraCascaded = std::make_shared<CameraCascadedFrustum<cascadeNum>>(cameraID, TE_CAMERA_TYPE::Orthographic, cascadeDepths, position, look, up, right);
				m_Map_CameraCascaded[cameraID] = cameraCascaded;

				camera = cameraCascaded.get();

				if (name == "")
				{
					auto prefix = "CameraCascaded";
					auto id = std::to_string(cameraID);
					std::string n = prefix + id;
					m_Map_CameraCascadedName[n] = cameraCascaded.get();
				}
				else
				{
					m_Map_CameraCascadedName[name] = cameraCascaded.get();
				}
			}
			return camera;
		}

}

#define TE_INSTANCE_CAMERAMANAGER TruthEngine::CameraManager::GetInstance()