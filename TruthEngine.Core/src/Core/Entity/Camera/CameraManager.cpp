#include "pch.h"
#include "CameraManager.h"

#include "CameraPerspective.h"
#include "CameraOrthographic.h"

using namespace DirectX;

namespace TruthEngine {

	namespace Core
	{		

		/*void CameraManager::AddCamera(std::shared_ptr<ICamera> camera)
		{
			m_Map_Cameras[camera->m_Name] = camera;

			if (m_ActiveCamera == nullptr)
			{
				m_ActiveCamera = camera;
				camera->Active();
			}
		}*/

		Camera* CameraManager::GetCamera(std::string_view name)
		{
			return &m_Map_Cameras[name];
		}


		CameraController* CameraManager::GetCameraController()
		{
			return &m_CameraController;
		}


		/*void CameraManager::SetActiveCamera(std::string_view name)
		{
			if (m_ActiveCamera != nullptr)
			{
				m_ActiveCamera->Deactive();
			}

			m_ActiveCamera = m_Map_Cameras[name];
			m_ActiveCamera->Active();
		}*/

		CameraManager* CameraManager::GetInstance()
		{
			static CameraManager s_Instance;
			return &s_Instance;
		}



		Camera* CameraManager::CreatePerspectiveFOV(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane)
		{
			auto vTarget = XMLoadFloat3(&target);
			auto vPosition = XMLoadFloat3(&position);
			auto vWorldUp = XMLoadFloat3(&worldUpVector);

			XMVECTOR L = XMVector3Normalize(XMVectorSubtract(vTarget, vPosition));
			XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
			XMVECTOR U = XMVector3Cross(L, R);

			float3 look, right, up;
			XMStoreFloat3(&look, L);
			XMStoreFloat3(&right, R);
			XMStoreFloat3(&up, U);


			auto nearWindowHeight = 2.0f * zNearPlane * tanf(0.5f * fovY);
			auto farWindowHeight = 2.0f * zFarPlane * tanf(0.5f * fovY);

			const auto nearWindowWidth = aspectRatio * nearWindowHeight;
			auto fovX = 2.0f * atanf(nearWindowWidth / 2.0f / zNearPlane);

			XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNearPlane, zFarPlane);
			float4x4 projMatrix;
			XMStoreFloat4x4(&projMatrix, P);

			m_Map_Cameras[name] = Camera(TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, fovX, nearWindowWidth, farWindowHeight, projMatrix);

			return &m_Map_Cameras[name];

		}


		Camera* CameraManager::CreatePerspective(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane)
		{
			auto vTarget = XMLoadFloat3(&target);
			auto vPosition = XMLoadFloat3(&position);
			auto vWorldUp = XMLoadFloat3(&worldUpVector);

			XMVECTOR L = XMVector3Normalize(XMVectorSubtract(vTarget, vPosition));
			XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
			XMVECTOR U = XMVector3Cross(L, R);

			float3 look, right, up;
			XMStoreFloat3(&look, L);
			XMStoreFloat3(&right, R);
			XMStoreFloat3(&up, U);

			float aspectRatio = viewWidth / viewHeight;
			float fovY = 2.0f * atanf(viewHeight / (2.0f * zNearPlane));

			auto farWindowHeight = 2.0f * zFarPlane * tanf(0.5f * fovY);

			auto fovX = 2.0f * atanf(viewWidth / 2.0f / zNearPlane);

			XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNearPlane, zFarPlane);
			float4x4 projMatrix;
			XMStoreFloat4x4(&projMatrix, P);

			m_Map_Cameras[name] = Camera(TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, fovX, viewWidth, farWindowHeight, projMatrix);

			return &m_Map_Cameras[name];
		}


		Camera* CameraManager::CreatePerspectiveCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane)
		{
			auto vTarget = XMLoadFloat3(&target);
			auto vPosition = XMLoadFloat3(&position);
			auto vWorldUp = XMLoadFloat3(&worldUpVector);

			XMVECTOR L = XMVector3Normalize(XMVectorSubtract(vTarget, vPosition));
			XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
			XMVECTOR U = XMVector3Cross(L, R);

			float3 look, right, up;
			XMStoreFloat3(&look, L);
			XMStoreFloat3(&right, R);
			XMStoreFloat3(&up, U);

			auto viewWidth = abs(viewRight - viewLeft);
			auto viewHeight = abs(viewTop - viewBottom);

			float aspectRatio = viewWidth / viewHeight;
			float fovY = 2.0f * atanf(viewHeight / (2.0f * zNearPlane));

			auto farWindowHeight = 2.0f * zFarPlane * tanf(0.5f * fovY);

			auto fovX = 2.0f * atanf(viewWidth / 2.0f / zNearPlane);

			XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNearPlane, zFarPlane);
			float4x4 projMatrix;
			XMStoreFloat4x4(&projMatrix, P);

			m_Map_Cameras[name] = Camera(TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, fovX, viewWidth, farWindowHeight, projMatrix);

			return &m_Map_Cameras[name];
		}


		Camera* CameraManager::CreateOrthographicCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane)
		{
			auto vTarget = XMLoadFloat3(&target);
			auto vPosition = XMLoadFloat3(&position);
			auto vWorldUp = XMLoadFloat3(&worldUpVector);

			XMVECTOR L = XMVector3Normalize(XMVectorSubtract(vTarget, vPosition));
			XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
			XMVECTOR U = XMVector3Cross(L, R);

			float3 look, right, up;
			XMStoreFloat3(&look, L);
			XMStoreFloat3(&right, R);
			XMStoreFloat3(&up, U);


			auto frustumWidth = abs(viewRight - viewLeft);
			auto frustumHeight = abs(viewTop - viewBottom);

			float4x4 projMatrix;
			XMStoreFloat4x4(&projMatrix, XMMatrixOrthographicLH(frustumWidth, frustumHeight, zNearPlane, zFarPlane));

			float aspectRatio = frustumWidth / frustumHeight;

			float fovY = 2.0f * atanf(frustumHeight / (2.0f * zNearPlane));

			float fovX = 2.0f * atanf(frustumWidth / (2.0f * zNearPlane));

			m_Map_Cameras[name] = Camera(TE_CAMERA_TYPE::Orthographic, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, fovX, frustumWidth, frustumHeight, projMatrix);

			return &m_Map_Cameras[name];
		}


		Camera* CameraManager::CreateOrthographic(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane)
		{
			auto vTarget = XMLoadFloat3(&target);
			auto vPosition = XMLoadFloat3(&position);
			auto vWorldUp = XMLoadFloat3(&worldUpVector);

			XMVECTOR L = XMVector3Normalize(XMVectorSubtract(vTarget, vPosition));
			XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
			XMVECTOR U = XMVector3Cross(L, R);

			float3 look, right, up;
			XMStoreFloat3(&look, L);
			XMStoreFloat3(&right, R);
			XMStoreFloat3(&up, U);


			float4x4 projMatrix;
			XMStoreFloat4x4(&projMatrix, XMMatrixOrthographicLH(viewWidth, viewHeight, zNearPlane, zFarPlane));

			float aspectRatio = viewWidth / viewHeight;

			float fovY = 2.0f * atanf(viewHeight / (2.0f * zNearPlane));

			float fovX = 2.0f * atanf(viewWidth / (2.0f * zNearPlane));

			m_Map_Cameras[name] = Camera(TE_CAMERA_TYPE::Orthographic, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, fovX, viewWidth, viewHeight, projMatrix);

			return &m_Map_Cameras[name];
		}

	}
}