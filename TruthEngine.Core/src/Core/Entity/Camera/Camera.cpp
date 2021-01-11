#include "pch.h"
#include "Camera.h"

using namespace DirectX;

namespace TruthEngine::Core
{

	Camera::Camera(TE_CAMERA_TYPE cameraType, const float3& position, const float3& look
		, const float3& up, const float3& right, const float zNear
		, const float zFar, const float aspectRatio, const float fovY, const float fovX
		, const float nearWindowHeight, const float farWindowHeight
		, const float4x4& projMatrix)
		: m_Position(position), m_Look(look), m_Up(up), m_Right(right)
		, m_ZNear(zNear), m_ZFar(zFar), m_AspectRatio(aspectRatio), m_FovY(fovY), m_FovX(fovX)
		, m_NearWindowHeight(nearWindowHeight), m_FarWindowHeight(farWindowHeight)
		, m_ProjectionMatrix(projMatrix)
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

		CreateBoundingFrustum();
	}


	void Camera::CreateBoundingFrustum()
	{
		auto XMView = XMLoadFloat4x4(&m_ViewMatrix);
		auto XMProj = XMLoadFloat4x4(&m_ProjectionMatrix);

		BoundingFrustum::CreateFromMatrix(m_BoundingFrustum, XMProj);
		const auto InvView = XMMatrixInverse(nullptr, XMView);
		m_BoundingFrustum.Transform(m_BoundingFrustum, InvView);
	}


}