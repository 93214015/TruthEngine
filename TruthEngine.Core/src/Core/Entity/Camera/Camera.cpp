#include "pch.h"
#include "Camera.h"

#include "CameraManager.h"

using namespace DirectX;

namespace TruthEngine
{

	Camera::Camera(uint32_t id, TE_CAMERA_TYPE cameraType, const float3& position, const float3& look
		, const float3& up, const float3& right, const float zNear
		, const float zFar, const float aspectRatio, const float fovY, const float fovX
		, const float nearWindowHeight, const float farWindowHeight
		, const float4x4& projMatrix, const BoundingFrustum& _BoundingFrustumView, bool _IsReveresedDepth, const std::function<void(Camera*)>& _FuncEditFrustum)
		: m_ID(id), m_CameraType(cameraType), m_Position(position), m_Look(look), m_Up(up), m_Right(right)
		, m_ZNear(zNear), m_ZFar(zFar), m_AspectRatio(aspectRatio), m_FovY(fovY), m_FovX(fovX)
		, m_NearWindowHeight(nearWindowHeight), m_FarWindowHeight(farWindowHeight)
		, m_ProjectionMatrix(projMatrix)
		, m_BoundingFrustumViewSpace(_BoundingFrustumView)
		, m_FuncEditFrustum(_FuncEditFrustum)
		, m_IsReversedDepth(_IsReveresedDepth)

	{
		UpdateViewMatrix();
	}



	void Camera::UpdateViewMatrix()
	{
		XMVECTOR R = XMLoadFloat3(&m_Right);
		XMVECTOR U = XMLoadFloat3(&m_Up);
		XMVECTOR L = XMLoadFloat3(&m_Look);
		XMVECTOR P = XMLoadFloat3(&m_Position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&m_Right, R);
		XMStoreFloat3(&m_Up, U);
		XMStoreFloat3(&m_Look, L);

		m_ViewMatrix(0, 0) = m_Right.x;
		m_ViewMatrix(1, 0) = m_Right.y;
		m_ViewMatrix(2, 0) = m_Right.z;
		m_ViewMatrix(3, 0) = x;

		m_ViewMatrix(0, 1) = m_Up.x;
		m_ViewMatrix(1, 1) = m_Up.y;
		m_ViewMatrix(2, 1) = m_Up.z;
		m_ViewMatrix(3, 1) = y;

		m_ViewMatrix(0, 2) = m_Look.x;
		m_ViewMatrix(1, 2) = m_Look.y;
		m_ViewMatrix(2, 2) = m_Look.z;
		m_ViewMatrix(3, 2) = z;

		m_ViewMatrix(0, 3) = 0.0f;
		m_ViewMatrix(1, 3) = 0.0f;
		m_ViewMatrix(2, 3) = 0.0f;
		m_ViewMatrix(3, 3) = 1.0f;

		m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;

		UpdateBoundingFrustumWorld();
	}


	void Camera::UpdateBoundingFrustumWorld()
	{
		auto XMView = XMLoadFloat4x4(&m_ViewMatrix);
		const auto InvView = XMMatrixInverse(nullptr, XMView);
		m_BoundingFrustumViewSpace.Transform(m_BoundingFrustumWorldSpace, InvView);
	}

	void Camera::SetFrustum(float width, float height, float zNearPlane, float zFarPlane)
	{
		m_ZNear = zNearPlane;
		m_ZFar = zFarPlane;
		m_NearWindowHeight = height;
		m_AspectRatio = width / height;

		m_FovY = 2.0f * atan(height / (2.0f * zNearPlane));
		m_FovX = 2.0f * atan(width / (2.0f * zNearPlane));

		m_FarWindowHeight = 2.0f * tan(m_FovY * 0.5f) * zFarPlane;

		m_FuncEditFrustum(this);

		m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;


	}

	void Camera::SetLook(const float3& _Look, const float3& _Up, const float3& _Right)
	{
		m_Look = _Look;
		m_Up = _Up;
		m_Right = _Right;
		UpdateViewMatrix();
	}

	void Camera::SetZNearPlane(const float zNearPlane)
	{
		m_ZNear = zNearPlane;

		m_FuncEditFrustum(this);

		m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
	}

	void Camera::SetZFarPlane(const float zFarPlane)
	{
		m_ZFar = zFarPlane;

		m_FuncEditFrustum(this);

		m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
	}

	void Camera::SetAspectRatio(const float aspectRatio)
	{
		m_AspectRatio = aspectRatio;

		m_FuncEditFrustum(this);

		m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
	}

	void Camera::SetFOVY(const float _FOVY)
	{
		m_FovY = _FOVY;

		m_FuncEditFrustum(this);

		m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
	}

}