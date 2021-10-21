#include "pch.h"
#include "CameraManager.h"

#include "CameraPerspective.h"
#include "CameraOrthographic.h"

#include "Core/Event/EventApplication.h"
#include "Core/Application.h"

using namespace DirectX;

namespace TruthEngine
{
	static const XMMatrix _MatReversingDepth
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	);
	/*void CameraManager::AddCamera(std::shared_ptr<ICamera> camera)
	{
		m_Map_Cameras[camera->m_Name] = camera;

		if (m_ActiveCamera == nullptr)
		{
			m_ActiveCamera = camera;
			camera->Active();
		}
	}*/

	CameraManager::CameraManager()
	{
		auto lambda_onViewportResize = [this](Event& event)
		{
			OnResizeViewport(static_cast<EventSceneViewportResize&>(event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::SceneViewportResize, lambda_onViewportResize);
	}

	Camera* CameraManager::GetCamera(std::string_view name)
	{
		auto itr = m_Map_CamerasName.find(name);
		if (itr != m_Map_CamerasName.end())
		{
			return itr->second;
		}

		return nullptr;
	}


	TruthEngine::CameraCascadedFrustumBase* CameraManager::GetCameraCascaded(std::string_view name)
	{
		auto itr = m_Map_CameraCascadedName.find(name);
		if (itr != m_Map_CameraCascadedName.end())
		{
			return itr->second;
		}

		return nullptr;
	}


	CameraController* CameraManager::GetCameraController()
	{
		return &m_CameraController;
	}


	TE_RESULT CameraManager::SetActiveCamera(const char* cameraName)
	{
		auto itr = m_Map_CamerasName.find(cameraName);

		if (itr == m_Map_CamerasName.end())
			return TE_RESULT_NOT_FOUND;

		m_ActiveCamera = itr->second;
		return TE_SUCCESSFUL;
	}


	TE_RESULT CameraManager::SetActiveCamera(Camera* camera)
	{
		m_ActiveCamera = camera;
		m_CameraController.AttachCamera(camera);
		return TE_SUCCESSFUL;
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
	

	void CameraManager::OnResizeViewport(const EventSceneViewportResize& event)
	{
		m_ActiveCamera->SetAspectRatio(static_cast<float>(event.GetWidth()) / static_cast<float>(event.GetHeight()));
	}

	void CameraManager::EditCameraFrustum(Camera* camera)
	{
		switch (m_ActiveCamera->m_CameraType)
		{
		case TE_CAMERA_TYPE::Perspective:
			EditCameraFrustumPerspective(m_ActiveCamera, camera->m_AspectRatio, camera->GetZNearPlane(), camera->GetZFarPlane());
			break;

		case TE_CAMERA_TYPE::Orthographic:
			EditCameraFrustumOrthographic(m_ActiveCamera, camera->GetZNearPlane(), camera->GetZFarPlane());
			break;
		default:
			break;
		}
	}

	void CameraManager::EditCameraFrustumPerspective(Camera* _Camera, float aspectRatio, float zNearPlane, float zFarPlane)
	{
		_Camera->m_ZNear = zNearPlane != -1.0f ? zNearPlane : _Camera->m_ZNear;
		_Camera->m_ZFar = zFarPlane != -1.0f ? zFarPlane : _Camera->m_ZFar;
		_Camera->m_AspectRatio = aspectRatio;

		EditCameraFrustumPerspective(_Camera);
	}

	void CameraManager::EditCameraFrustumPerspective(Camera* _Camera)
	{
		XMMATRIX P = XMMatrixPerspectiveFovLH(_Camera->m_FovY, _Camera->m_AspectRatio, _Camera->m_ZNear, _Camera->m_ZFar);

		BoundingFrustum::CreateFromMatrix(_Camera->m_BoundingFrustumViewSpace, P);

		if (_Camera->m_IsReversedDepth)
		{
			P = P * _MatReversingDepth;
		}
		XMStoreFloat4x4A(&_Camera->m_ProjectionMatrix, P);

		XMMatrix _ViewInv = Math::ToXM(_Camera->m_ViewInvMatrix);
		_Camera->m_BoundingFrustumViewSpace.Transform(_Camera->m_BoundingFrustumWorldSpace, _ViewInv);

	}

	void CameraManager::EditCameraFrustumOrthographic(Camera* camera, float aspectRatio, float zNearPlane, float zFarPlane)
	{
		throw;

		// TODO: Fix Orthographic edit frustum function

		/*camera->m_ZNear = zNearPlane != -1.0f ? zNearPlane : camera->m_ZNear;
		camera->m_ZFar = zFarPlane != -1.0f ? zFarPlane : camera->m_ZFar;
		camera->m_AspectRatio = aspectRatio;

		camera->m_FarWindowHeight = viewHeight;
		camera->m_NearWindowHeight = viewHeight;

		XMMATRIX P = XMMatrixOrthographicLH(viewWidth, viewHeight, camera->m_ZNear, camera->m_ZFar);
		XMStoreFloat4x4(&camera->m_ProjectionMatrix, P);*/
	}

	void CameraManager::EditCameraFrustumOrthographic(Camera* camera)
	{
		throw;
	}

	Camera* CameraManager::CreatePerspectiveFOV(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane, bool _IsReversedDepth)
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

		XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNearPlane, zFarPlane);

		BoundingFrustum _BoundingFrustum;
		BoundingFrustum::CreateFromMatrix(_BoundingFrustum, P);

		if (_IsReversedDepth)
		{
			P = P * _MatReversingDepth;
		}
		float4x4A projMatrix;
		XMStoreFloat4x4A(&projMatrix, P);


		Camera* _Camera = GetCamera(name);
		if (_Camera) // if camera with the same name was existed then we recreate that in the place
		{
			*_Camera = Camera(_Camera->m_ID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective);
		}
		else
		{
			uint32_t _CameraID = static_cast<uint32_t>(m_Cameras.size());
			_Camera = &m_Cameras.emplace_back(Camera(_CameraID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective));
			
			if (name == "")
			{
				std::string prefix = "CameraPerspective";
				std::string id = std::to_string(_CameraID);
				prefix += id;
				m_Map_CamerasName[prefix] = _Camera;
			}
			else
			{
				m_Map_CamerasName[name] = _Camera;
			}
		}
		return _Camera;
	}

	Camera* CameraManager::CreatePerspectiveTarget(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane, bool _IsReversedDepth)
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

		XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNearPlane, zFarPlane);

		BoundingFrustum _BoundingFrustum;
		BoundingFrustum::CreateFromMatrix(_BoundingFrustum, P);

		if (_IsReversedDepth)
		{
			P = P * _MatReversingDepth;
		}
		float4x4A projMatrix;
		XMStoreFloat4x4A(&projMatrix, P);


		Camera* _Camera = GetCamera(name);
		if (_Camera) // if camera with the same name was existed then we recreate that in the place
		{
			*_Camera = Camera(_Camera->m_ID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective);
		}
		else
		{
			uint32_t cameraID = static_cast<uint32_t>(m_Cameras.size());
			_Camera = &m_Cameras.emplace_back(Camera(cameraID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective));

			if (name == "")
			{
				auto prefix = "CameraPerspective";
				auto id = std::to_string(cameraID);
				std::string n = prefix + id;
				m_Map_CamerasName[n] = _Camera;
			}
			else
			{
				m_Map_CamerasName[name] = _Camera;
			}
		}
		return _Camera;
	}

	Camera* CameraManager::CreatePerspectiveDirection(const char* name, const float3& position, const float3& look, const float3& worldUpVector, float fovY, float aspectRatio, float zNearPlane, float zFarPlane, bool _IsReversedDepth)
	{
		auto vPosition = XMLoadFloat3(&position);
		auto vWorldUp = XMLoadFloat3(&worldUpVector);

		XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&look));
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
		XMVECTOR U = XMVector3Cross(L, R);

		float3 right, up;
		XMStoreFloat3(&right, R);
		XMStoreFloat3(&up, U);

		XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNearPlane, zFarPlane);

		BoundingFrustum _BoundingFrustum;
		BoundingFrustum::CreateFromMatrix(_BoundingFrustum, P);

		if (_IsReversedDepth)
		{
			P = P * _MatReversingDepth;
		}

		float4x4A projMatrix;
		XMStoreFloat4x4A(&projMatrix, P);

		Camera* _Camera = GetCamera(name);
		if (_Camera) // if camera with the same name was existed then we recreate that in the place
		{
			*_Camera = Camera(_Camera->m_ID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective);
		}
		else
		{
			uint32_t cameraID = static_cast<uint32_t>(m_Cameras.size());
			_Camera = &m_Cameras.emplace_back(Camera(cameraID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective));
			
			if (name == "")
			{
				auto prefix = "CameraPerspective";
				auto id = std::to_string(cameraID);
				std::string n = prefix + id;
				m_Map_CamerasName[n] = _Camera;
			}
			else
			{
				m_Map_CamerasName[name] = _Camera;
			}
		}

		return _Camera;
	}

	Camera* CameraManager::CreatePerspectiveCenterOff(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane, bool _IsReversedDepth)
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

		XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNearPlane, zFarPlane);

		BoundingFrustum _BoundingFrustum;
		BoundingFrustum::CreateFromMatrix(_BoundingFrustum, P);

		if (_IsReversedDepth)
		{
			P = P * _MatReversingDepth;
		}
		float4x4A projMatrix;
		XMStoreFloat4x4A(&projMatrix, P);

		Camera* _Camera = GetCamera(name);
		if (_Camera) // if camera with the same name was existed then we recreate that in the place
		{
			*_Camera = Camera(_Camera->m_ID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective);
		}
		else
		{

			uint32_t cameraID = static_cast<uint32_t>(m_Cameras.size());
			_Camera = &m_Cameras.emplace_back(Camera(cameraID, TE_CAMERA_TYPE::Perspective, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, _IsReversedDepth, &CameraManager::EditCameraFrustumPerspective));

			if (name == "")
			{
				auto prefix = "CameraPerspective";
				auto id = std::to_string(cameraID);
				std::string n = prefix + id;
				m_Map_CamerasName[n] = _Camera;
			}
			else
			{
				m_Map_CamerasName[name] = _Camera;
			}
		}
		return _Camera;
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


		const XMMatrix P = XMMatrixOrthographicLH(frustumWidth, frustumHeight, zNearPlane, zFarPlane);
		float4x4A projMatrix;
		XMStoreFloat4x4A(&projMatrix, P);

		BoundingFrustum _BoundingFrustum;

		if (zNearPlane < zFarPlane)
		{
			BoundingFrustum::CreateFromMatrix(_BoundingFrustum, P);
		}
		else
		{
			const XMMATRIX _PTemp = XMMatrixOrthographicLH(frustumWidth, frustumHeight, zFarPlane, zNearPlane);
			BoundingFrustum::CreateFromMatrix(_BoundingFrustum, _PTemp);
		}


		float aspectRatio = frustumWidth / frustumHeight;

		float fovY = 2.0f * atanf(frustumHeight / (2.0f * zNearPlane));

		Camera* _Camera = GetCamera(name);
		if (_Camera) // if camera with the same name was existed then we recreate that in the place
		{
			*_Camera = Camera(_Camera->m_ID, TE_CAMERA_TYPE::Orthographic, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, false, &CameraManager::EditCameraFrustumOrthographic);
		}
		else
		{
			uint32_t cameraID = static_cast<uint32_t>(m_Cameras.size());
			_Camera = &m_Cameras.emplace_back(Camera(cameraID, TE_CAMERA_TYPE::Orthographic, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, false, &CameraManager::EditCameraFrustumOrthographic));


			if (name == "")
			{
				auto prefix = "CameraOrthographic";
				auto id = std::to_string(cameraID);
				std::string n = prefix + id;
				m_Map_CamerasName[n] = _Camera;
			}
			else
			{
				m_Map_CamerasName[name] = _Camera;
			}
		}
		return _Camera;
	}

	Camera* CameraManager::CreateOrthographicTarget(const char* name, const float3& position, const float3& target, const float3& worldUpVector, float viewWidth, float viewHeight, float zNearPlane, float zFarPlane)
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

		const XMMatrix P = XMMatrixOrthographicLH(viewWidth, viewHeight, zNearPlane, zFarPlane);
		float4x4A projMatrix;
		XMStoreFloat4x4A(&projMatrix, P);

		BoundingFrustum _BoundingFrustum;
		if (zNearPlane < zFarPlane)
		{
			BoundingFrustum::CreateFromMatrix(_BoundingFrustum, P);
		}
		else
		{
			const XMMATRIX _PTemp = XMMatrixOrthographicLH(viewWidth, viewHeight, zFarPlane, zNearPlane);
			BoundingFrustum::CreateFromMatrix(_BoundingFrustum, _PTemp);
		}


		float aspectRatio = viewWidth / viewHeight;

		float fovY = 2.0f * atanf(viewHeight / (2.0f * zNearPlane));

		Camera* _Camera = GetCamera(name);
		if (_Camera) // if camera with the same name was existed then we recreate that in the place
		{
			*_Camera = Camera(_Camera->m_ID, TE_CAMERA_TYPE::Orthographic, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, false, &CameraManager::EditCameraFrustumOrthographic);
		}
		else
		{
			uint32_t cameraID = static_cast<uint32_t>(m_Cameras.size());
			_Camera = &m_Cameras.emplace_back(Camera(cameraID, TE_CAMERA_TYPE::Orthographic, position, look, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, false, &CameraManager::EditCameraFrustumOrthographic));

			if (name == "")
			{
				auto prefix = "CameraOrthographic";
				auto id = std::to_string(cameraID);
				std::string n = prefix + id;
				m_Map_CamerasName[n] = _Camera;
			}
			else
			{
				m_Map_CamerasName[name] = _Camera;
			}
		}
		return _Camera;
	}

	Camera* CameraManager::CreateOrthographicDirection(const char* name, const float3& position, const float3& direction, const float3& worldUpVector, float viewLeft, float viewTop, float viewRight, float viewBottom, float zNearPlane, float zFarPlane)
	{
		auto vPosition = XMLoadFloat3(&position);
		auto vWorldUp = XMLoadFloat3(&worldUpVector);

		XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&direction));
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(vWorldUp, L));
		XMVECTOR U = XMVector3Cross(L, R);

		float3 right, up;
		XMStoreFloat3(&right, R);
		XMStoreFloat3(&up, U);

		auto frustumWidth = abs(viewRight - viewLeft);
		auto frustumHeight = abs(viewTop - viewBottom);

		const XMMatrix P = XMMatrixOrthographicLH(frustumWidth, frustumHeight, zNearPlane, zFarPlane);
		float4x4A projMatrix;
		XMStoreFloat4x4A(&projMatrix, P);

		float aspectRatio = frustumWidth / frustumHeight;

		float fovY = 2.0f * atanf(frustumHeight / (2.0f * zNearPlane));

		BoundingFrustum _BoundingFrustum;
		if (zNearPlane < zFarPlane)
		{
			BoundingFrustum::CreateFromMatrix(_BoundingFrustum, P);
		}
		else
		{
			const XMMATRIX _PTemp = XMMatrixOrthographicLH(frustumWidth, frustumHeight, zFarPlane, zNearPlane);
			BoundingFrustum::CreateFromMatrix(_BoundingFrustum, _PTemp);
		}

		Camera* _Camera = GetCamera(name);
		if (_Camera) // if camera with the same name was existed then we recreate that in the place
		{
			*_Camera = Camera(_Camera->m_ID, TE_CAMERA_TYPE::Orthographic, position, direction, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, false, &CameraManager::EditCameraFrustumOrthographic);
		}
		else
		{
			uint32_t cameraID = static_cast<uint32_t>(m_Cameras.size());
			_Camera = &m_Cameras.emplace_back(Camera(cameraID, TE_CAMERA_TYPE::Orthographic, position, direction, up, right, zNearPlane, zFarPlane, aspectRatio, fovY, projMatrix, _BoundingFrustum, false, &CameraManager::EditCameraFrustumOrthographic));

			if (name == "")
			{
				auto prefix = "CameraOrthographic";
				auto id = std::to_string(cameraID);
				std::string n = prefix + id;
				m_Map_CamerasName[n] = _Camera;
			}
			else
			{
				m_Map_CamerasName[name] = _Camera;
			}
		}
		return _Camera;
	}


}